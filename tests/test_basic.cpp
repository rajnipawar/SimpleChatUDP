#include <QtTest/QtTest>
#include "../src/message.h"

class TestBasic : public QObject {
    Q_OBJECT

private slots:
    void testMessageCreation() {
        Message msg("Hello", "Node1", "Node2", 1);
        QCOMPARE(msg.getChatText(), QString("Hello"));
        QCOMPARE(msg.getOrigin(), QString("Node1"));
        QCOMPARE(msg.getDestination(), QString("Node2"));
        QCOMPARE(msg.getSequenceNumber(), 1);
        QVERIFY(msg.isValid());
    }

    void testMessageSerialization() {
        Message original("Test message", "Node1", "Node2", 5);
        QByteArray datagram = original.toDatagram();
        Message deserialized = Message::fromDatagram(datagram);

        QCOMPARE(deserialized.getChatText(), original.getChatText());
        QCOMPARE(deserialized.getOrigin(), original.getOrigin());
        QCOMPARE(deserialized.getDestination(), original.getDestination());
        QCOMPARE(deserialized.getSequenceNumber(), original.getSequenceNumber());
    }

    void testBroadcastMessage() {
        Message msg("Broadcast test", "Node1", "broadcast", 1);
        QVERIFY(msg.isBroadcast());

        Message msg2("P2P test", "Node1", "Node2", 1);
        QVERIFY(!msg2.isBroadcast());
    }

    void testMessageId() {
        Message msg("Test", "Node1", "Node2", 42);
        QString expectedId = "Node1_42";
        QCOMPARE(msg.getMessageId(), expectedId);
    }

    void testVectorClock() {
        Message msg("Test", "Node1", "Node2", 1);
        QVariantMap vc;
        vc["Node1"] = 5;
        vc["Node2"] = 3;
        msg.setVectorClock(vc);

        QVariantMap retrieved = msg.getVectorClock();
        QCOMPARE(retrieved.value("Node1").toInt(), 5);
        QCOMPARE(retrieved.value("Node2").toInt(), 3);
    }
};

QTEST_MAIN(TestBasic)
#include "test_basic.moc"
