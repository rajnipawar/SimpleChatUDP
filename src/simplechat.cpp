#include "simplechat.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

const QList<int> SimpleChat::DEFAULT_PORTS = {9001, 9002, 9003, 9004};

SimpleChat::SimpleChat(int port, const QList<int>& peerPorts, QObject* parent)
    : QObject(parent), serverPort(port) {

    nodeId = generateNodeId(port);

    window = new ChatWindow();
    window->setNodeId(nodeId);

    networkManager = new NetworkManager(this);
    networkManager->setNodeId(nodeId);

    connect(window, &ChatWindow::messageEntered, this, &SimpleChat::onMessageEntered);
    connect(networkManager, &NetworkManager::messageReceived, this, &SimpleChat::onMessageReceived);
    connect(networkManager, &NetworkManager::peerDiscovered, this, &SimpleChat::onPeerDiscovered);
    connect(networkManager, &NetworkManager::peerStatusChanged, this, &SimpleChat::onPeerStatusChanged);

    if (!networkManager->startServer(port)) {
        QMessageBox::critical(nullptr, "Error", QString("Failed to start server on port %1").arg(port));
        QApplication::exit(1);
        return;
    }

    // Use provided peer ports or defaults
    discoveryPorts = peerPorts.isEmpty() ? DEFAULT_PORTS : peerPorts;

    window->appendMessage(QString("SimpleChat P2P Node %1 started on port %2").arg(nodeId).arg(port));
    window->appendMessage("Features: Peer-to-Peer messaging, Broadcast, Anti-Entropy sync");
    window->appendMessage("Discovering peers...");

    // Start peer discovery
    setupPeerDiscovery();
}

SimpleChat::~SimpleChat() {
    delete window;
}

void SimpleChat::show() {
    window->show();
}

QString SimpleChat::generateNodeId(int port) {
    int nodeNumber = DEFAULT_PORTS.indexOf(port);
    if (nodeNumber >= 0) {
        return QString("Node%1").arg(nodeNumber + 1);
    }
    return QString("Node%1").arg(port);
}

void SimpleChat::setupPeerDiscovery() {
    // Discover peers on specified ports
    networkManager->discoverLocalPeers(discoveryPorts);

    // Also manually add known peers (for deterministic setup)
    for (int port : discoveryPorts) {
        if (port != serverPort) {
            QString peerId = generateNodeId(port);
            networkManager->addPeer(peerId, "127.0.0.1", port);
        }
    }

    window->appendMessage(QString("Attempting to discover peers on ports: %1")
        .arg(QString::number(discoveryPorts[0]) + "-" + QString::number(discoveryPorts.last())));
}

void SimpleChat::onMessageEntered(const QString& text, const QString& destination) {
    QString trimmedText = text.trimmed();
    if (trimmedText.isEmpty()) {
        window->appendMessage("Message cannot be empty.");
        return;
    }

    if (destination.isEmpty()) {
        window->appendMessage("Please select a destination.");
        return;
    }

    // Create message
    Message message(trimmedText, nodeId, destination, 1);
    qDebug() << "Sending message from" << nodeId << "to" << destination << ":" << trimmedText;
    networkManager->sendMessage(message);

    // Add to conversation
    if (destination == "broadcast" || destination == "-1") {
        // Show broadcast in UI
        window->appendSentMessage("broadcast", trimmedText);
    } else {
        window->appendSentMessage(destination, trimmedText);
    }
}

void SimpleChat::onMessageReceived(const Message& message) {
    QString origin = message.getOrigin();
    QString text = message.getChatText();

    if (message.isBroadcast()) {
        // Show in broadcast tab
        window->appendReceivedMessage("broadcast", QString("[%1]: %2").arg(origin).arg(text));
        window->appendMessage(QString("Broadcast from %1: %2").arg(origin).arg(text));
    } else {
        // Show in peer-specific tab
        window->appendReceivedMessage(origin, text);
        window->appendMessage(QString("Message from %1: %2").arg(origin).arg(text));
    }

    qDebug() << "Message delivered from" << origin << ":" << text;
}

void SimpleChat::onPeerDiscovered(const QString& peerId, const QString& host, int port) {
    window->appendMessage(QString("Discovered peer: %1 at %2:%3").arg(peerId).arg(host).arg(port));

    // Update peer list in UI
    QList<QString> activePeers = networkManager->getActivePeers();
    window->updatePeerList(activePeers);
}

void SimpleChat::onPeerStatusChanged(const QString& peerId, bool active) {
    QString status = active ? "active" : "inactive";
    window->appendMessage(QString("Peer %1 is now %2").arg(peerId).arg(status));

    // Update peer status in UI
    window->updatePeerStatus(peerId, active);

    // Update peer list
    QList<QString> activePeers = networkManager->getActivePeers();
    window->updatePeerList(activePeers);
}
