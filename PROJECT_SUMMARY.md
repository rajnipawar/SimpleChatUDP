# SimpleChat P2P - Project Summary

## Student Information
- **Name:** Rajni Pawar
- **Assignment:** Programming Assignment 2
- **Course:** CS 550 - Advanced Operating Systems
- **Semester:** Fall 2025

## Implementation Summary

This project implements a peer-to-peer chat application with the following key features:

### 1. Network Architecture Change
- **Before (PA1):** TCP-based ring topology using QTcpSocket
- **After (PA2):** UDP-based peer-to-peer mesh network using QUdpSocket
- Messages are sent directly to target peers, no intermediate routing
- Supports both unicast (P2P) and broadcast messaging

### 2. Core Features Implemented

#### UDP Communication
- Replaced QTcpSocket/QTcpServer with QUdpSocket
- Message serialization using QVariantMap converted to JSON
- Datagram-based communication with custom message format
- Support for localhost communication on ports 9001-9004

#### Broadcast Messaging
- Special destination identifier: "broadcast" or "-1"
- Messages sent to all active peers simultaneously
- Dedicated broadcast tab in UI
- Broadcast messages shown with sender identification

#### Anti-Entropy Protocol
- **Vector Clock Implementation:**
  - Each node maintains a map: `{origin -> max_sequence_number}`
  - Tracks highest sequence number seen from each node
  - Used to identify missing messages

- **Synchronization Process:**
  - Periodic anti-entropy exchanges (every 2 seconds)
  - Random peer selection for efficiency
  - Vector clock comparison to find gaps
  - Automatic transmission of missing messages
  - Ensures eventual consistency across all nodes

- **Message Types:**
  - `ANTI_ENTROPY_REQUEST`: Initiates sync with vector clock
  - `ANTI_ENTROPY_RESPONSE`: Response with local vector clock
  - Missing messages sent as regular `CHAT_MESSAGE` types

#### Reliable Delivery
- **ACK/Retry Mechanism:**
  - ACK timeout: 2 seconds
  - Maximum retries: 3 attempts
  - Exponential backoff (optional enhancement)
  - Pending message tracking with timestamps

- **Message Tracking:**
  - Unique message IDs: `{origin}_{sequence}`
  - Deduplication based on message ID
  - Message store for anti-entropy sync
  - Sequence number ordering per origin

#### Peer Discovery
- **Local Discovery:**
  - Scans specified port range (9001-9004 by default)
  - Discovery messages sent to potential peer ports
  - Automatic peer registration on first message

- **Peer Management:**
  - Active peer list with online/offline status
  - Peer health monitoring (timeout: 15 seconds)
  - Last-seen timestamp tracking
  - Dynamic peer status updates in UI

### 3. Message Structure

```cpp
Message {
    QString chatText;           // Message content
    QString origin;             // Sender node ID
    QString destination;        // Target node ID (or "broadcast")
    int sequenceNumber;         // Sequence per destination
    MessageType type;           // CHAT/ANTI_ENTROPY/ACK
    QVariantMap vectorClock;    // {origin -> max_seq}
    QString messageId;          // Unique ID: origin_seq
}
```

### 4. GUI Enhancements

#### Layout
- **Left Panel:** Active peers list with status indicators
- **Right Panel:** Tabbed chat interface
- **Tabs:**
  - System: General messages and logs
  - Broadcast: Broadcast messages
  - Per-peer: Individual conversation tabs (created dynamically)

#### Styling
- Dark theme inspired by modern messaging apps
- Color-coded message bubbles:
  - Sent messages: Blue (#007AFF)
  - Received messages: Gray (#2A2F32)
- Peer status indicators (green=online, gray=offline)
- Modern rounded buttons and inputs

### 5. Key Algorithms

#### Vector Clock Comparison
```
LocalVC: {Node1: 5, Node2: 3, Node3: 7}
RemoteVC: {Node1: 3, Node2: 4, Node3: 5}

Missing messages to send:
- Node1: sequences 4, 5
- Node3: sequences 6, 7

Missing messages to request:
- Node2: sequence 4
```

#### Anti-Entropy Synchronization
```
1. Node A selects random peer Node B
2. Node A sends ANTI_ENTROPY_REQUEST with VectorClock_A
3. Node B compares VectorClock_A with VectorClock_B
4. Node B identifies missing messages
5. Node B sends ANTI_ENTROPY_RESPONSE with VectorClock_B
6. Node B sends missing messages to Node A
7. Node A receives response and sends its missing messages to Node B
8. Both nodes update their vector clocks
```

#### Retry Logic
```
1. Send message to peer
2. Store in pendingAcks with timestamp
3. Start ACK check timer (1 second interval)
4. On ACK received: remove from pendingAcks
5. On timeout (2 seconds):
   a. If retries < MAX_RETRIES (3): resend and increment retry count
   b. If retries >= MAX_RETRIES: remove and log failure
```

## File Structure and Responsibilities

### Core Files

**message.h/cpp**
- Message data structure and serialization
- Support for different message types
- Vector clock management
- JSON serialization/deserialization

**networkmanager.h/cpp**
- UDP socket management
- Message sending/receiving
- Peer discovery and management
- Anti-entropy protocol implementation
- ACK/retry mechanism
- Peer health monitoring

**chatwindow.h/cpp**
- Qt GUI implementation
- Tabbed interface for conversations
- Peer list display
- Message display with styling
- User input handling

**simplechat.h/cpp**
- Main application controller
- Connects GUI and network components
- Message routing
- Event handling

**main.cpp**
- Application entry point
- Command-line argument parsing
- Application initialization

### Supporting Files

**CMakeLists.txt**
- Build configuration
- Qt dependency management
- Compiler settings

**scripts/**
- build.sh: Automated build process
- launch_2_nodes.sh: Launch 2 instances
- launch_4_nodes.sh: Launch 4 instances
- stop_all.sh: Stop all running instances

**tests/**
- test_basic.cpp: Unit tests for Message class
- CMakeLists.txt: Test build configuration

## Testing Performed

### Manual Testing
1. ✅ 2-node P2P messaging
2. ✅ 4-node P2P messaging
3. ✅ Broadcast to all peers
4. ✅ Message ordering verification
5. ✅ Peer discovery
6. ✅ Peer status updates (online/offline)
7. ✅ Anti-entropy synchronization
8. ✅ Message retry on failure
9. ✅ Multi-hop propagation (via anti-entropy)

### Unit Tests
- Message creation and validation
- Message serialization/deserialization
- Broadcast message detection
- Message ID generation
- Vector clock operations

## Challenges and Solutions

### Challenge 1: UDP Reliability
**Problem:** UDP doesn't guarantee delivery
**Solution:** Implemented ACK/retry mechanism with timeout and max retries

### Challenge 2: Message Ordering
**Problem:** UDP packets can arrive out of order
**Solution:** Sequence numbers per origin with message store and reordering

### Challenge 3: Eventual Consistency
**Problem:** Messages might not reach all peers initially
**Solution:** Anti-entropy protocol with vector clocks ensures all nodes eventually sync

### Challenge 4: Peer Discovery
**Problem:** How to find peers without central server
**Solution:** Local port scanning + manual peer configuration

### Challenge 5: Duplicate Messages
**Problem:** Retries and anti-entropy could cause duplicates
**Solution:** Message ID-based deduplication in message store

## Performance Considerations

- **Anti-entropy interval:** 2 seconds (tunable)
- **ACK timeout:** 2 seconds (tunable)
- **Max retries:** 3 (tunable)
- **Peer health check:** 5 seconds (tunable)
- **Peer timeout:** 15 seconds (tunable)

## Future Improvements

1. **Scalability:** Support for more than 4 nodes
2. **WAN Support:** NAT traversal and remote IP addresses
3. **Security:** Message encryption and authentication
4. **Persistence:** SQLite database for message history
5. **Advanced UI:** Timestamps, read receipts, typing indicators
6. **File Transfer:** Support for attachments
7. **Group Chat:** Multi-peer conversations
8. **Mobile Support:** Qt Quick/QML interface

## Lessons Learned

1. **UDP Programming:** Understanding connectionless protocols and implementing reliability
2. **Distributed Systems:** Vector clocks, anti-entropy, eventual consistency
3. **Qt Framework:** Signal/slot mechanism, timers, network programming
4. **P2P Architecture:** Decentralized communication patterns
5. **CMake Build System:** Cross-platform C++ project configuration

## Conclusion

This project successfully implements a peer-to-peer chat application with broadcast and anti-entropy features. The UDP-based architecture provides efficient communication while custom reliability mechanisms ensure message delivery. The anti-entropy protocol with vector clocks enables automatic synchronization and eventual consistency across all nodes.

The implementation demonstrates understanding of:
- Distributed systems concepts
- Network programming with UDP
- Qt GUI framework
- Build automation
- Testing strategies

## References

1. Qt6 Documentation - QUdpSocket: https://doc.qt.io/qt-6/qudpsocket.html
2. Vector Clocks - Leslie Lamport: "Time, Clocks, and the Ordering of Events in a Distributed System"
3. Anti-Entropy Protocol: "Epidemic Algorithms for Replicated Database Maintenance" (Demers et al.)
4. UDP vs TCP: "Computer Networks" by Andrew S. Tanenbaum
5. CMake Documentation: https://cmake.org/documentation/

---

**Date Completed:** October 13, 2025
**Total Implementation Time:** ~6 hours
**Lines of Code:** ~2,100
**Git Commits:** 2
