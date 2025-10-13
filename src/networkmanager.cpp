#include "networkmanager.h"
#include <QHostAddress>
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent), socket(nullptr), serverPort(0) {

    socket = new QUdpSocket(this);
    connect(socket, &QUdpSocket::readyRead, this, &NetworkManager::onDataReceived);

    // Anti-entropy timer for periodic synchronization
    antiEntropyTimer = new QTimer(this);
    connect(antiEntropyTimer, &QTimer::timeout, this, &NetworkManager::onAntiEntropyTimeout);

    // ACK check timer for reliable delivery
    ackCheckTimer = new QTimer(this);
    connect(ackCheckTimer, &QTimer::timeout, this, &NetworkManager::checkPendingAcks);

    // Peer health check timer
    peerHealthTimer = new QTimer(this);
    connect(peerHealthTimer, &QTimer::timeout, this, &NetworkManager::checkPeerHealth);
}

NetworkManager::~NetworkManager() {
    if (socket) {
        socket->close();
    }
}

bool NetworkManager::startServer(int port) {
    if (!socket->bind(QHostAddress::LocalHost, port)) {
        qDebug() << "Failed to bind UDP socket on port" << port << ":" << socket->errorString();
        return false;
    }

    serverPort = port;
    qDebug() << "UDP server started on port" << port;

    // Start timers
    antiEntropyTimer->start(ANTI_ENTROPY_INTERVAL);
    ackCheckTimer->start(ACK_CHECK_INTERVAL);
    peerHealthTimer->start(PEER_HEALTH_CHECK_INTERVAL);

    return true;
}

void NetworkManager::addPeer(const QString& peerId, const QString& host, int port) {
    if (peerId == nodeId) {
        return;  // Don't add self as peer
    }

    PeerInfo peerInfo(peerId, host, port);
    peers[peerId] = peerInfo;

    qDebug() << "Added peer:" << peerId << "at" << host << ":" << port;
    emit peerDiscovered(peerId, host, port);
}

void NetworkManager::discoverLocalPeers(const QList<int>& portRange) {
    qDebug() << "Discovering peers on local ports:" << portRange;

    // Send discovery message to each port
    for (int port : portRange) {
        if (port == serverPort) {
            continue;  // Skip own port
        }

        Message discoveryMsg("", nodeId, "discovery", 0, Message::ANTI_ENTROPY_REQUEST);
        QByteArray datagram = discoveryMsg.toDatagram();
        sendDatagram(datagram, QHostAddress::LocalHost, port);
    }
}

void NetworkManager::sendMessage(const Message& message) {
    if (!message.isValid() && message.getType() != Message::ANTI_ENTROPY_REQUEST) {
        qDebug() << "Invalid message, not sending";
        return;
    }

    Message msgToSend = message;
    msgToSend.setOrigin(nodeId);

    // Assign sequence number for chat messages
    if (msgToSend.getType() == Message::CHAT_MESSAGE) {
        const QString& destination = msgToSend.getDestination();
        QString seqKey = destination;

        if (!nextSequenceNumbers.contains(seqKey)) {
            nextSequenceNumbers[seqKey] = 1;
        }

        msgToSend.setSequenceNumber(nextSequenceNumbers[seqKey]++);
        msgToSend.setMessageId(msgToSend.generateMessageId());

        // Update own vector clock
        updateVectorClock(nodeId, msgToSend.getSequenceNumber());

        // Store the message
        storeMessage(msgToSend);
    }

    // Set vector clock
    msgToSend.setVectorClock(vectorClock);

    qDebug() << "Sending message from" << msgToSend.getOrigin()
             << "to" << msgToSend.getDestination()
             << "seq:" << msgToSend.getSequenceNumber()
             << "type:" << msgToSend.getType();

    if (msgToSend.isBroadcast()) {
        sendBroadcastMessage(msgToSend);
    } else {
        sendDirectMessage(msgToSend, msgToSend.getDestination());
    }
}

void NetworkManager::sendDirectMessage(const Message& message, const QString& peerId) {
    if (!peers.contains(peerId)) {
        qDebug() << "Unknown peer:" << peerId;
        return;
    }

    PeerInfo& peer = peers[peerId];
    QByteArray datagram = message.toDatagram();
    sendDatagram(datagram, QHostAddress(peer.host), peer.port);

    // For chat messages, track for ACK (only if not already tracking - avoid overwriting during retries)
    if (message.getType() == Message::CHAT_MESSAGE && !pendingAcks.contains(message.getMessageId())) {
        PendingMessage pending;
        pending.message = message;
        pending.targetPeerId = peerId;
        pending.sentTime = QDateTime::currentMSecsSinceEpoch();
        pending.retryCount = 0;

        pendingAcks[message.getMessageId()] = pending;
    }
}

void NetworkManager::sendBroadcastMessage(const Message& message) {
    int totalPeers = peers.size();
    int activePeers = 0;

    qDebug() << "Broadcasting message to all peers. Total peers:" << totalPeers;

    for (auto it = peers.begin(); it != peers.end(); ++it) {
        const PeerInfo& peer = it.value();
        if (peer.isActive) {
            activePeers++;
            QByteArray datagram = message.toDatagram();
            sendDatagram(datagram, QHostAddress(peer.host), peer.port);
        }
    }

    qDebug() << "Broadcast sent to" << activePeers << "active peers out of" << totalPeers << "total";

    // For broadcast chat messages, we don't track ACKs (gossip-style)
}

void NetworkManager::sendDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port) {
    qint64 sent = socket->writeDatagram(datagram, host, port);
    if (sent == -1) {
        qDebug() << "Failed to send datagram:" << socket->errorString();
    }
}

void NetworkManager::onDataReceived() {
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress senderHost;
        quint16 senderPort;

        qint64 received = socket->readDatagram(datagram.data(), datagram.size(), &senderHost, &senderPort);

        if (received > 0) {
            Message message = Message::fromDatagram(datagram);

            if (message.getOrigin() == nodeId) {
                // Ignore messages from self
                continue;
            }

            processReceivedMessage(message, senderHost, senderPort);
        }
    }
}

void NetworkManager::processReceivedMessage(const Message& message, const QHostAddress& senderHost, quint16 senderPort) {
    // Update peer info
    QString senderId = message.getOrigin();
    if (!peers.contains(senderId)) {
        addPeer(senderId, senderHost.toString(), senderPort);
    } else {
        peers[senderId].lastSeen = QDateTime::currentMSecsSinceEpoch();
        if (!peers[senderId].isActive) {
            peers[senderId].isActive = true;
            emit peerStatusChanged(senderId, true);
        }
    }

    switch (message.getType()) {
        case Message::CHAT_MESSAGE:
            handleChatMessage(message);
            break;
        case Message::ANTI_ENTROPY_REQUEST:
            handleAntiEntropyRequest(message, senderHost, senderPort);
            break;
        case Message::ANTI_ENTROPY_RESPONSE:
            handleAntiEntropyResponse(message);
            break;
        case Message::ACK:
            handleAck(message);
            break;
    }
}

void NetworkManager::handleChatMessage(const Message& message) {
    // Check if this is for us or broadcast
    bool isForUs = message.getDestination() == nodeId || message.isBroadcast();

    // Store message if we haven't seen it
    if (!hasMessage(message.getMessageId())) {
        qDebug() << "New message from" << message.getOrigin()
                 << "to" << message.getDestination()
                 << "seq:" << message.getSequenceNumber();

        storeMessage(message);
        updateVectorClock(message.getOrigin(), message.getSequenceNumber());

        // Deliver if it's for us
        if (isForUs) {
            emit messageReceived(message);
        }

        // Send ACK if it's directly to us (not broadcast)
        if (message.getDestination() == nodeId) {
            Message ack("", nodeId, message.getOrigin(), 0, Message::ACK);
            ack.setMessageId(message.getMessageId());
            sendDirectMessage(ack, message.getOrigin());
        }
    }
}

void NetworkManager::handleAntiEntropyRequest(const Message& message, const QHostAddress& senderHost, quint16 senderPort) {
    QString senderId = message.getOrigin();

    // Compare vector clocks
    QVariantMap remoteVectorClock = message.getVectorClock();
    QList<Message> missingMessages = getMissingMessages(remoteVectorClock);

    // Only log if there are missing messages
    if (!missingMessages.isEmpty()) {
        qDebug() << "Anti-entropy: Sending" << missingMessages.size() << "missing messages to" << senderId;
    }

    // Send response with our vector clock
    Message response("", nodeId, senderId, 0, Message::ANTI_ENTROPY_RESPONSE);
    response.setVectorClock(vectorClock);

    // Include missing messages in the response
    // For simplicity, we send them as separate messages
    sendDatagram(response.toDatagram(), senderHost, senderPort);

    for (const Message& msg : missingMessages) {
        sendDatagram(msg.toDatagram(), senderHost, senderPort);
    }
}

void NetworkManager::handleAntiEntropyResponse(const Message& message) {
    // Update our knowledge of what the peer has
    QVariantMap remoteVectorClock = message.getVectorClock();

    // Send missing messages to the peer
    QList<Message> missingMessages = getMissingMessages(remoteVectorClock);

    // Only log if there are missing messages
    if (!missingMessages.isEmpty()) {
        qDebug() << "Anti-entropy: Sending" << missingMessages.size() << "missing messages to" << message.getOrigin();
    }

    for (const Message& msg : missingMessages) {
        sendDirectMessage(msg, message.getOrigin());
    }
}

void NetworkManager::handleAck(const Message& message) {
    QString messageId = message.getMessageId();

    if (pendingAcks.contains(messageId)) {
        qDebug() << "Received ACK for message" << messageId;
        pendingAcks.remove(messageId);
    }
}

void NetworkManager::onAntiEntropyTimeout() {
    performAntiEntropy();
}

void NetworkManager::performAntiEntropy() {
    if (peers.isEmpty()) {
        return;
    }

    // Send anti-entropy request to a random active peer
    QList<QString> activePeerIds;
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        if (it.value().isActive) {
            activePeerIds.append(it.key());
        }
    }

    if (activePeerIds.isEmpty()) {
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(activePeerIds.size());
    QString randomPeerId = activePeerIds[randomIndex];

    Message request("", nodeId, randomPeerId, 0, Message::ANTI_ENTROPY_REQUEST);
    request.setVectorClock(vectorClock);

    // Silent - don't log routine anti-entropy
    sendDirectMessage(request, randomPeerId);
}

void NetworkManager::checkPendingAcks() {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QList<QString> toRetry;

    for (auto it = pendingAcks.begin(); it != pendingAcks.end(); ) {
        PendingMessage& pending = it.value();

        if (now - pending.sentTime > ACK_TIMEOUT) {
            if (pending.retryCount < MAX_RETRIES) {
                qDebug() << "Retry sending message" << pending.message.getMessageId()
                         << "attempt" << (pending.retryCount + 1);
                toRetry.append(it.key());
                ++it;
            } else {
                qDebug() << "Message" << pending.message.getMessageId() << "failed after" << MAX_RETRIES << "retries";
                it = pendingAcks.erase(it);
            }
        } else {
            ++it;
        }
    }

    // Retry messages
    for (const QString& messageId : toRetry) {
        PendingMessage& pending = pendingAcks[messageId];
        pending.retryCount++;
        pending.sentTime = now;
        sendDirectMessage(pending.message, pending.targetPeerId);
    }
}

void NetworkManager::checkPeerHealth() {
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    for (auto it = peers.begin(); it != peers.end(); ++it) {
        PeerInfo& peer = it.value();

        if (peer.isActive && (now - peer.lastSeen > PEER_TIMEOUT)) {
            qDebug() << "Peer" << peer.peerId << "timed out";
            peer.isActive = false;
            emit peerStatusChanged(peer.peerId, false);
        }
    }
}

void NetworkManager::updateVectorClock(const QString& origin, int sequenceNumber) {
    int currentMax = vectorClock.value(origin, 0).toInt();
    if (sequenceNumber > currentMax) {
        vectorClock[origin] = sequenceNumber;
    }
}

bool NetworkManager::hasMessage(const QString& messageId) const {
    return messageStore.contains(messageId);
}

void NetworkManager::storeMessage(const Message& message) {
    messageStore[message.getMessageId()] = message;
}

QList<Message> NetworkManager::getMissingMessages(const QVariantMap& remoteVectorClock) const {
    QList<Message> missing;

    // Find messages that the remote peer doesn't have
    for (auto it = messageStore.begin(); it != messageStore.end(); ++it) {
        const Message& msg = it.value();
        QString origin = msg.getOrigin();
        int localSeq = msg.getSequenceNumber();
        int remoteSeq = remoteVectorClock.value(origin, 0).toInt();

        if (localSeq > remoteSeq) {
            missing.append(msg);
        }
    }

    return missing;
}

QList<QString> NetworkManager::getActivePeers() const {
    QList<QString> activePeers;
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        if (it.value().isActive) {
            activePeers.append(it.key());
        }
    }
    return activePeers;
}

QString NetworkManager::findPeerIdByAddress(const QHostAddress& host, quint16 port) const {
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        const PeerInfo& peer = it.value();
        if (peer.host == host.toString() && peer.port == port) {
            return peer.peerId;
        }
    }
    return QString();
}
