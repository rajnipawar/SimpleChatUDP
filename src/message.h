#pragma once

#include <QVariantMap>
#include <QString>
#include <QDataStream>

class Message {
public:
    enum MessageType {
        CHAT_MESSAGE,
        ANTI_ENTROPY_REQUEST,
        ANTI_ENTROPY_RESPONSE,
        ACK
    };

    Message();
    Message(const QString& chatText, const QString& origin, const QString& destination, int sequenceNumber, MessageType type = CHAT_MESSAGE);

    static Message fromVariantMap(const QVariantMap& map);
    static Message fromDatagram(const QByteArray& datagram);
    QVariantMap toVariantMap() const;
    QByteArray toDatagram() const;

    QString getChatText() const { return chatText; }
    QString getOrigin() const { return origin; }
    QString getDestination() const { return destination; }
    int getSequenceNumber() const { return sequenceNumber; }
    MessageType getType() const { return type; }
    QVariantMap getVectorClock() const { return vectorClock; }
    QString getMessageId() const { return messageId; }

    void setChatText(const QString& text) { chatText = text; }
    void setOrigin(const QString& org) { origin = org; }
    void setDestination(const QString& dest) { destination = dest; }
    void setSequenceNumber(int seq) { sequenceNumber = seq; }
    void setType(MessageType t) { type = t; }
    void setVectorClock(const QVariantMap& vc) { vectorClock = vc; }
    void setMessageId(const QString& id) { messageId = id; }

    bool isValid() const;
    bool isBroadcast() const { return destination == "-1" || destination == "broadcast"; }

    QString generateMessageId() const;

private:
    QString chatText;
    QString origin;
    QString destination;  // "-1" or "broadcast" indicates broadcast message
    int sequenceNumber;
    MessageType type;
    QVariantMap vectorClock;  // For anti-entropy: origin -> max sequence number
    QString messageId;  // Unique identifier: origin_sequence
};

QDataStream& operator<<(QDataStream& stream, const Message& message);
QDataStream& operator>>(QDataStream& stream, Message& message);
