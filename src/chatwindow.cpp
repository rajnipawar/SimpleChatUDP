#include "chatwindow.h"
#include <QApplication>
#include <QKeyEvent>

ChatWindow::ChatWindow(QWidget* parent) : QWidget(parent) {
    setupUI();
    setWindowTitle("SimpleChat P2P");
    resize(700, 500);
}

void ChatWindow::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);

    // Apply dark theme styling to main window
    setStyleSheet("QWidget { background-color: #0B141A; color: #E9EDEF; }");

    nodeLabel = new QLabel("Node: Unknown", this);
    nodeLabel->setStyleSheet(
        "font-weight: bold; "
        "color: #00D4AA; "
        "background-color: #202C33; "
        "padding: 12px 16px; "
        "border-radius: 16px;"
    );
    mainLayout->addWidget(nodeLabel);

    // Chat area (no splitter, no peer list sidebar)
    QWidget* chatContainer = new QWidget(this);
    auto* chatLayout = new QVBoxLayout(chatContainer);

    conversationTabs = new QTabWidget(this);
    conversationTabs->setStyleSheet(
        "QTabWidget::pane { background-color: #0B141A; border: none; } "
        "QTabBar::tab { background-color: #202C33; color: #8696A0; padding: 10px 18px; margin: 3px; border-radius: 12px; } "
        "QTabBar::tab:selected { background-color: #00D4AA; color: #0B141A; font-weight: bold; border-radius: 12px; }"
    );

    // System tab for general messages
    systemLog = new QTextEdit(this);
    systemLog->setReadOnly(true);
    systemLog->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    systemLog->setStyleSheet(
        "background-color: #0B141A; "
        "color: #8696A0; "
        "border: 1px solid #202C33; "
        "border-radius: 12px; "
        "padding: 12px; "
        "font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
    );
    conversationTabs->addTab(systemLog, "System");

    // Broadcast tab
    QTextEdit* broadcastLog = new QTextEdit(this);
    broadcastLog->setReadOnly(true);
    broadcastLog->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    broadcastLog->setStyleSheet(systemLog->styleSheet());
    conversationTabs->addTab(broadcastLog, "Broadcast");
    conversations["broadcast"] = broadcastLog;

    // Set System tab as default
    conversationTabs->setCurrentIndex(0);

    chatLayout->addWidget(conversationTabs);

    // Connect tab change signal
    connect(conversationTabs, &QTabWidget::currentChanged, this, &ChatWindow::onTabChanged);

    // Create input container
    inputContainer = new QWidget(this);
    inputContainer->setStyleSheet(
        "QWidget { "
        "    background-color: #1E293B; "
        "    border-top: 1px solid #374151; "
        "    padding: 8px; "
        "    border-radius: 0px 0px 12px 12px; "
        "}"
    );
    auto* inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(12);

    // Destination selection
    destLabel = new QLabel("To:", this);
    destLabel->setStyleSheet(
        "color: #8696A0; "
        "font-weight: bold; "
        "padding: 8px;"
    );
    inputLayout->addWidget(destLabel);

    destinationCombo = new QComboBox(this);
    destinationCombo->setMinimumWidth(120);
    destinationCombo->setMinimumHeight(40);
    destinationCombo->setStyleSheet(
        "QComboBox { "
        "    background-color: #202C33; "
        "    color: #E9EDEF; "
        "    padding: 12px 18px; "
        "    border: 1px solid #4B5563; "
        "    border-radius: 12px; "
        "    font-weight: 500; "
        "    font-size: 14px; "
        "    min-height: 20px; "
        "    outline: none; "
        "}"
        "QComboBox:hover { "
        "    border-color: #00D4AA; "
        "    background-color: #374151; "
        "}"
        "QComboBox:focus { "
        "    border-color: #00D4AA; "
        "    border-width: 2px; "
        "}"
        "QComboBox::drop-down { "
        "    border: none; "
        "    width: 30px; "
        "    background: transparent; "
        "}"
        "QComboBox::down-arrow { "
        "    width: 0; "
        "    height: 0; "
        "    border-left: 6px solid transparent; "
        "    border-right: 6px solid transparent; "
        "    border-top: 8px solid #00D4AA; "
        "    margin-right: 8px; "
        "}"
        "QComboBox QAbstractItemView { "
        "    background-color: #202C33; "
        "    color: #E9EDEF; "
        "    selection-background-color: #00D4AA; "
        "    selection-color: #000000; "
        "    border: 2px solid #00D4AA; "
        "    border-radius: 12px; "
        "    padding: 6px; "
        "    font-size: 14px; "
        "    outline: none; "
        "}"
        "QComboBox QAbstractItemView::item { "
        "    padding: 12px 16px; "
        "    border-radius: 8px; "
        "    margin: 2px; "
        "    background: transparent; "
        "    min-height: 25px; "
        "}"
        "QComboBox QAbstractItemView::item:hover { "
        "    background-color: rgba(0, 212, 170, 0.2); "
        "    color: #FFFFFF; "
        "}"
        "QComboBox QAbstractItemView::item:selected { "
        "    background-color: #00D4AA; "
        "    color: #000000; "
        "    font-weight: 600; "
        "}"
    );
    inputLayout->addWidget(destinationCombo);

    messageInput = new QTextEdit(this);
    messageInput->setPlaceholderText("Type your message here...");
    messageInput->setMaximumHeight(80);
    messageInput->setMinimumHeight(50);
    messageInput->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    messageInput->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageInput->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    messageInput->setStyleSheet(
        "QTextEdit { "
        "    background-color: #374151; "
        "    color: #FFFFFF; "
        "    padding: 8px 14px; "
        "    border: 2px solid #6B7280; "
        "    border-radius: 16px; "
        "    font-family: Arial, sans-serif; "
        "    font-size: 14px; "
        "    line-height: 1.2; "
        "    selection-background-color: #00D4AA; "
        "    selection-color: #0B141A; "
        "} "
        "QTextEdit:focus { "
        "    border-color: #00D4AA; "
        "    border-width: 2px; "
        "    background-color: #475569; "
        "} "
        "QTextEdit:hover { "
        "    border-color: #9CA3AF; "
        "    background-color: #475569; "
        "} "
        "QScrollBar:vertical { "
        "    background: rgba(55, 65, 81, 0.5); "
        "    width: 8px; "
        "    border-radius: 4px; "
        "    margin: 2px; "
        "} "
        "QScrollBar::handle:vertical { "
        "    background: rgba(156, 163, 175, 0.7); "
        "    border-radius: 4px; "
        "    min-height: 20px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "    background: rgba(156, 163, 175, 0.9); "
        "} "
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "    height: 0px; "
        "} "
        "QScrollBar:horizontal { "
        "    height: 0px; "
        "}"
    );
    connect(messageInput, &QTextEdit::textChanged, this, [this]() {
        // Auto-resize based on content
        QTextDocument* doc = messageInput->document();
        int height = doc->size().height() + 16; // Add padding
        messageInput->setFixedHeight(qMax(50, qMin(80, height)));
    });

    // Install event filter to handle Enter key
    messageInput->installEventFilter(this);
    inputLayout->addWidget(messageInput);

    sendButton = new QPushButton("Send", this);
    sendButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #00D4AA; "
        "    color: #0B141A; "
        "    padding: 14px 28px; "
        "    border: none; "
        "    border-radius: 16px; "
        "    font-weight: bold; "
        "    font-family: Arial, sans-serif; "
        "    font-size: 14px; "
        "    min-height: 20px; "
        "}"
        "QPushButton:hover { "
        "    background-color: #00B894; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #00A085; "
        "}"
    );
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
    inputLayout->addWidget(sendButton);

    broadcastButton = new QPushButton("Broadcast", this);
    broadcastButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #FF6B6B; "
        "    color: #FFFFFF; "
        "    padding: 14px 28px; "
        "    border: none; "
        "    border-radius: 16px; "
        "    font-weight: bold; "
        "    font-family: Arial, sans-serif; "
        "    font-size: 14px; "
        "    min-height: 20px; "
        "}"
        "QPushButton:hover { "
        "    background-color: #FF5252; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #FF3838; "
        "}"
    );
    connect(broadcastButton, &QPushButton::clicked, this, &ChatWindow::onBroadcastClicked);
    inputLayout->addWidget(broadcastButton);

    chatLayout->addWidget(inputContainer);

    mainLayout->addWidget(chatContainer);

    // Initially update input visibility
    updateInputVisibility();

    messageInput->setFocus();
}

void ChatWindow::appendMessage(const QString& message) {
    systemLog->append(message);
    systemLog->moveCursor(QTextCursor::End);
}

void ChatWindow::appendMessageToConversation(const QString& nodeId, const QString& message) {
    QTextEdit* conversation = getOrCreateConversation(nodeId);
    conversation->append(message);
    conversation->moveCursor(QTextCursor::End);
}

void ChatWindow::appendSentMessage(const QString& nodeId, const QString& message) {
    QTextEdit* conversation = getOrCreateConversation(nodeId);
    QString styledMessage = QString(
        "<table width='100%' cellpadding='0' cellspacing='0' style='margin: 8px 0;'>"
        "<tr>"
        "<td width='30%'></td>"
        "<td align='right'>"
        "<div style='"
        "background-color: #007AFF; "
        "color: #FFFFFF; "
        "padding: 12px 16px; "
        "border-radius: 18px 18px 6px 18px; "
        "font-size: 14px; "
        "line-height: 1.4; "
        "font-weight: 400; "
        "white-space: pre-wrap; "
        "display: inline-block; "
        "max-width: 250px; "
        "word-wrap: break-word; "
        "font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Arial, sans-serif; "
        "box-shadow: 0 1px 2px rgba(0,0,0,0.1); "
        "'>%1</div>"
        "</td>"
        "</tr>"
        "</table>"
    ).arg(message);
    conversation->append(styledMessage);
    conversation->moveCursor(QTextCursor::End);
}

void ChatWindow::appendReceivedMessage(const QString& nodeId, const QString& message) {
    QTextEdit* conversation = getOrCreateConversation(nodeId);
    QString styledMessage = QString(
        "<table width='100%' cellpadding='0' cellspacing='0' style='margin: 8px 0;'>"
        "<tr>"
        "<td align='left'>"
        "<div style='"
        "background-color: #2A2F32; "
        "color: #E9EDEF; "
        "padding: 12px 16px; "
        "border-radius: 18px 18px 18px 6px; "
        "font-size: 14px; "
        "line-height: 1.4; "
        "font-weight: normal; "
        "white-space: pre-wrap; "
        "display: inline-block; "
        "max-width: 250px; "
        "word-wrap: break-word; "
        "font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Arial, sans-serif; "
        "box-shadow: 0 1px 2px rgba(0,0,0,0.1); "
        "'>%1</div>"
        "</td>"
        "<td width='30%'></td>"
        "</tr>"
        "</table>"
    ).arg(message);
    conversation->append(styledMessage);
    conversation->moveCursor(QTextCursor::End);
}

QTextEdit* ChatWindow::getOrCreateConversation(const QString& nodeId) {
    QString conversationKey = nodeId;

    if (conversations.contains(conversationKey)) {
        return conversations[conversationKey];
    }

    // Create new conversation tab
    QTextEdit* newConversation = new QTextEdit(this);
    newConversation->setReadOnly(true);
    newConversation->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    newConversation->setStyleSheet(
        "background-color: #0B141A; "
        "color: #8696A0; "
        "border: 1px solid #202C33; "
        "border-radius: 12px; "
        "padding: 20px; "
        "font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
    );

    QString tabName = QString("%1").arg(conversationKey);
    conversationTabs->addTab(newConversation, tabName);
    conversations[conversationKey] = newConversation;

    return newConversation;
}

void ChatWindow::setNodeId(const QString& nodeId) {
    currentNodeId = nodeId;
    nodeLabel->setText(QString("Node: %1 (P2P Mode)").arg(nodeId));
    setWindowTitle(QString("SimpleChat P2P - %1").arg(nodeId));
}

QString ChatWindow::getSelectedDestination() const {
    return destinationCombo->currentText();
}

void ChatWindow::updatePeerList(const QList<QString>& peers) {
    // Update combo box only
    QString currentSelection = destinationCombo->currentText();
    destinationCombo->clear();

    for (const QString& peer : peers) {
        if (peer != currentNodeId) {
            destinationCombo->addItem(peer);
        }
    }

    // Restore previous selection if still available
    if (!currentSelection.isEmpty()) {
        int index = destinationCombo->findText(currentSelection);
        if (index >= 0) {
            destinationCombo->setCurrentIndex(index);
        }
    }
}

void ChatWindow::updatePeerStatus(const QString& peerId, bool active) {
    peerStatus[peerId] = active;
    // Just track status, UI updates through updatePeerList
}

void ChatWindow::onSendClicked() {
    QString text = messageInput->toPlainText().trimmed();
    QString destination = getCurrentTabDestination();
    if (!text.isEmpty() && !destination.isEmpty()) {
        emit messageEntered(text, destination);
        messageInput->clear();
    }
}

void ChatWindow::onBroadcastClicked() {
    QString text = messageInput->toPlainText().trimmed();
    if (!text.isEmpty()) {
        emit messageEntered(text, "broadcast");
        // Message display is handled in simplechat.cpp to avoid duplication
        messageInput->clear();
    }
}

void ChatWindow::onReturnPressed() {
    onSendClicked();
}

void ChatWindow::keyPressEvent(QKeyEvent* event) {
    QWidget::keyPressEvent(event);
}

bool ChatWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == messageInput && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                // Shift+Enter creates new line - let QTextEdit handle it
                return false;
            } else {
                // Regular Enter sends message
                onSendClicked();
                return true; // Consume the event
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ChatWindow::onTabChanged(int index) {
    updateInputVisibility();
}

void ChatWindow::updateInputVisibility() {
    bool isSystemTab = (conversationTabs->currentIndex() == 0);
    bool isBroadcastTab = (conversationTabs->currentIndex() == 1);

    destLabel->setVisible(isSystemTab);
    destinationCombo->setVisible(isSystemTab);
    sendButton->setVisible(!isBroadcastTab);
    broadcastButton->setVisible(isBroadcastTab || isSystemTab);
}

QString ChatWindow::getCurrentTabDestination() const {
    int currentIndex = conversationTabs->currentIndex();
    if (currentIndex == 0) {
        // System tab - use dropdown selection
        return getSelectedDestination();
    } else if (currentIndex == 1) {
        // Broadcast tab
        return "broadcast";
    } else {
        // Node-specific tab - extract node name from tab text
        QString tabText = conversationTabs->tabText(currentIndex);
        return tabText;
    }
}
