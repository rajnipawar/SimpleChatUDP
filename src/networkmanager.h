#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QQueue>
#include <QPair>
#include <QDateTime>
#include "message.h"

struct PeerInfo {
    QString peerId;
    QString host;
    int port;
    bool isActive;
    qint64 lastSeen;

    PeerInfo() : port(0), isActive(false), lastSeen(0) {}
    PeerInfo(const QString& id, const QString& h, int p)
        : peerId(id), host(h), port(p), isActive(true), lastSeen(QDateTime::currentMSecsSinceEpoch()) {}
};

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    bool startServer(int port);
    void sendMessage(const Message& message);
    void addPeer(const QString& peerId, const QString& host, int port);
    void discoverLocalPeers(const QList<int>& portRange);

    void setNodeId(const QString& nodeId) { this->nodeId = nodeId; }
    QString getNodeId() const { return nodeId; }

    QList<QString> getActivePeers() const;
    QVariantMap getVectorClock() const { return vectorClock; }

signals:
    void messageReceived(const Message& message);
    void peerDiscovered(const QString& peerId, const QString& host, int port);
    void peerStatusChanged(const QString& peerId, bool active);

private slots:
    void onDataReceived();
    void onAntiEntropyTimeout();
    void checkPendingAcks();
    void checkPeerHealth();

private:
    void processReceivedMessage(const Message& message, const QHostAddress& senderHost, quint16 senderPort);
    void handleChatMessage(const Message& message);
    void handleAntiEntropyRequest(const Message& message, const QHostAddress& senderHost, quint16 senderPort);
    void handleAntiEntropyResponse(const Message& message);
    void handleAck(const Message& message);

    void sendDirectMessage(const Message& message, const QString& peerId, bool requireAck = true);
    void sendBroadcastMessage(const Message& message);
    void sendWithRetry(const Message& message, const QString& peerId);
    void sendDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port);

    void updateVectorClock(const QString& origin, int sequenceNumber);
    void performAntiEntropy();
    void syncMissingMessages(const QString& peerId);

    bool hasMessage(const QString& messageId) const;
    void storeMessage(const Message& message);
    QList<Message> getMissingMessages(const QVariantMap& remoteVectorClock) const;

    QString findPeerIdByAddress(const QHostAddress& host, quint16 port) const;

    QUdpSocket* socket;
    QString nodeId;
    int serverPort;

    // Peer management
    QMap<QString, PeerInfo> peers;  // peerId -> PeerInfo
    QTimer* antiEntropyTimer;
    QTimer* ackCheckTimer;
    QTimer* peerHealthTimer;

    // Message management
    QMap<QString, Message> messageStore;  // messageId -> Message
    QVariantMap vectorClock;  // origin -> max sequence number seen

    // Reliable delivery
    struct PendingMessage {
        Message message;
        QString targetPeerId;
        qint64 sentTime;
        int retryCount;
    };
    QMap<QString, PendingMessage> pendingAcks;  // messageId -> PendingMessage
    QMap<QString, int> nextSequenceNumbers;  // destination -> next sequence number

    // Configuration
    static const int ANTI_ENTROPY_INTERVAL = 2000;  // 2 seconds
    static const int ACK_CHECK_INTERVAL = 1000;  // 1 second
    static const int ACK_TIMEOUT = 2000;  // 2 seconds
    static const int MAX_RETRIES = 3;
    static const int PEER_HEALTH_CHECK_INTERVAL = 5000;  // 5 seconds
    static const int PEER_TIMEOUT = 15000;  // 15 seconds
};
