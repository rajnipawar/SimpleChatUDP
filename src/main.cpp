#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "simplechat.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName("SimpleChat P2P");
    QApplication::setApplicationVersion("2.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("SimpleChat - Peer-to-Peer Messaging Application with Broadcast and Anti-Entropy");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  "Port number for this node (9001-9004)", "port", "9001");
    parser.addOption(portOption);

    QCommandLineOption peersOption(QStringList() << "peers",
                                   "Comma-separated list of peer ports (e.g., 9001,9002,9003,9004)", "peers");
    parser.addOption(peersOption);

    parser.process(app);

    bool ok;
    int port = parser.value(portOption).toInt(&ok);

    if (!ok || port < 1024 || port > 65535) {
        qDebug() << "Invalid port number. Using default port 9001.";
        port = 9001;
    }

    // Parse peer ports if provided
    QList<int> peerPorts;
    if (parser.isSet(peersOption)) {
        QString peersStr = parser.value(peersOption);
        QStringList peersList = peersStr.split(',');
        for (const QString& peerStr : peersList) {
            int peerPort = peerStr.trimmed().toInt(&ok);
            if (ok && peerPort >= 1024 && peerPort <= 65535) {
                peerPorts.append(peerPort);
            }
        }
    }

    SimpleChat chat(port, peerPorts);
    chat.show();

    return app.exec();
}
