#include "chatwindow.h"
#include <QApplication>

ChatWindow::ChatWindow(QWidget* parent) : QWidget(parent) {
    setupUI();
    setWindowTitle("SimpleChat P2P");
    resize(600, 500);
}

void ChatWindow::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Node label at top
    nodeLabel = new QLabel("Node: Unknown", this);
    nodeLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; padding: 8px; background: #ecf0f1; border-radius: 5px;");
    mainLayout->addWidget(nodeLabel);

    // Peer list label
    peerListLabel = new QLabel("Active Peers: None", this);
    peerListLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; padding: 5px;");
    mainLayout->addWidget(peerListLabel);

    // Chat display area (takes most space)
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 10px;"
        "    font-family: Arial, sans-serif;"
        "    font-size: 13px;"
        "}"
    );
    mainLayout->addWidget(chatDisplay, 1);  // Stretch factor 1

    // Destination selection
    auto* destLayout = new QHBoxLayout();
    QLabel* destLabel = new QLabel("Send to:", this);
    destLabel->setStyleSheet("font-size: 12px; font-weight: bold;");
    destLayout->addWidget(destLabel);

    destinationCombo = new QComboBox(this);
    destinationCombo->setMinimumWidth(150);
    destinationCombo->setStyleSheet(
        "QComboBox {"
        "    padding: 5px 10px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 3px;"
        "    background: white;"
        "}"
    );
    destLayout->addWidget(destinationCombo);
    destLayout->addStretch();
    mainLayout->addLayout(destLayout);

    // Message input area
    auto* inputLayout = new QHBoxLayout();
    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Type your message here...");
    messageInput->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 3px;"
        "    font-size: 13px;"
        "}"
    );
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::onSendClicked);
    inputLayout->addWidget(messageInput, 1);

    sendButton = new QPushButton("Send", this);
    sendButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    padding: 8px 20px;"
        "    border: none;"
        "    border-radius: 3px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
    inputLayout->addWidget(sendButton);

    broadcastButton = new QPushButton("Broadcast", this);
    broadcastButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    padding: 8px 20px;"
        "    border: none;"
        "    border-radius: 3px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a93226;"
        "}"
    );
    connect(broadcastButton, &QPushButton::clicked, this, &ChatWindow::onBroadcastClicked);
    inputLayout->addWidget(broadcastButton);

    mainLayout->addLayout(inputLayout);

    messageInput->setFocus();
}

void ChatWindow::appendMessage(const QString& message) {
    chatDisplay->append(message);
    chatDisplay->moveCursor(QTextCursor::End);
}

void ChatWindow::appendSentMessage(const QString& nodeId, const QString& message) {
    QString formatted = QString("<div style='margin: 5px 0; color: #2c3e50;'>"
                                "<b style='color: #27ae60;'>You → %1:</b> %2"
                                "</div>").arg(nodeId).arg(message);
    chatDisplay->append(formatted);
    chatDisplay->moveCursor(QTextCursor::End);
}

void ChatWindow::appendReceivedMessage(const QString& nodeId, const QString& message) {
    QString formatted = QString("<div style='margin: 5px 0; color: #2c3e50;'>"
                                "<b style='color: #2980b9;'>%1:</b> %2"
                                "</div>").arg(nodeId).arg(message);
    chatDisplay->append(formatted);
    chatDisplay->moveCursor(QTextCursor::End);
}

void ChatWindow::setNodeId(const QString& nodeId) {
    currentNodeId = nodeId;
    nodeLabel->setText(QString("Node: %1").arg(nodeId));
    setWindowTitle(QString("SimpleChat P2P - %1").arg(nodeId));
}

QString ChatWindow::getSelectedDestination() const {
    return destinationCombo->currentText();
}

void ChatWindow::updatePeerList(const QList<QString>& peers) {
    QString currentSelection = destinationCombo->currentText();
    destinationCombo->clear();

    QStringList peerList;
    for (const QString& peer : peers) {
        if (peer != currentNodeId) {
            destinationCombo->addItem(peer);
            peerList.append(peer);
        }
    }

    // Update peer list label
    if (peerList.isEmpty()) {
        peerListLabel->setText("Active Peers: None");
    } else {
        peerListLabel->setText(QString("Active Peers: %1").arg(peerList.join(", ")));
    }

    // Restore previous selection if available
    if (!currentSelection.isEmpty()) {
        int index = destinationCombo->findText(currentSelection);
        if (index >= 0) {
            destinationCombo->setCurrentIndex(index);
        }
    }
}

void ChatWindow::updatePeerStatus(const QString& peerId, bool active) {
    QString status = active ? "online" : "offline";
    appendMessage(QString("📡 Peer %1 is now %2").arg(peerId).arg(status));
}

void ChatWindow::onSendClicked() {
    QString text = messageInput->text().trimmed();
    QString destination = destinationCombo->currentText();

    if (text.isEmpty()) {
        appendMessage("⚠ Message cannot be empty.");
        return;
    }

    if (destination.isEmpty()) {
        appendMessage("⚠ Please select a destination.");
        return;
    }

    emit messageEntered(text, destination);
    appendSentMessage(destination, text);
    messageInput->clear();
    messageInput->setFocus();
}

void ChatWindow::onBroadcastClicked() {
    QString text = messageInput->text().trimmed();

    if (text.isEmpty()) {
        appendMessage("⚠ Message cannot be empty.");
        return;
    }

    emit messageEntered(text, "broadcast");
    appendMessage(QString("<div style='margin: 5px 0;'>"
                          "<b style='color: #e74c3c;'>📢 BROADCAST:</b> %1"
                          "</div>").arg(text));
    messageInput->clear();
    messageInput->setFocus();
}
