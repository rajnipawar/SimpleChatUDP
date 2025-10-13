# SimpleChat P2P - Programming Assignment 2

**Student:** Rajni Pawar
**Course:** CS 550 - Advanced Operating Systems
**Semester:** Fall 2025

## Overview

SimpleChat P2P is a peer-to-peer messaging application with broadcast capabilities and anti-entropy synchronization. This implementation extends the original ring-based SimpleChat to support direct peer-to-peer communication using UDP, broadcast messaging, and automatic message synchronization.

## Key Features

### Network Communication
- **UDP-based messaging** using QUdpSocket instead of TCP
- **Message serialization/deserialization** using QVariantMap and JSON
- **Local peer discovery** on specified port ranges
- **Reliable delivery** with ACK/retry mechanism (1-2 second timeout)

### Messaging Protocol
- **Direct peer-to-peer messaging** - No ring topology required
- **Broadcast messaging** - Send messages to all connected peers simultaneously
- **Anti-Entropy synchronization** - Periodic exchange of message histories using vector clocks
- **Message ordering** - Proper sequencing for both P2P and broadcast messages
- **Retry mechanism** - Automatic retransmission of unacknowledged messages

### Message Structure
Each message contains:
- **ChatText**: The actual message content
- **Origin**: Unique identifier for the sending node
- **Destination**: Target node ID (or "-1"/"broadcast" for broadcast messages)
- **SequenceNumber**: For message ordering
- **MessageId**: Unique identifier (origin_sequence)
- **VectorClock**: Tracks message history across all nodes
- **Type**: Message type (CHAT_MESSAGE, ANTI_ENTROPY_REQUEST, ANTI_ENTROPY_RESPONSE, ACK)

### Anti-Entropy Protocol
The anti-entropy mechanism ensures reliable message propagation:
- Each node maintains a **vector clock** tracking the highest sequence number seen from each origin
- Nodes periodically (every 2 seconds) exchange vector clocks with random peers
- Missing messages are identified by comparing vector clocks
- Nodes request and transmit missing messages to achieve consistency
- This ensures messages propagate across multiple hops even if some transmissions fail

### Peer Discovery
- Automatic discovery of peers on local ports
- Manual peer addition via IP/hostname
- Peer health monitoring with timeout detection
- Dynamic peer status updates in UI

### User Interface
- **Modern dark-themed Qt GUI** with improved styling
- **System tab** - General system messages and status updates
- **Broadcast tab** - View and send broadcast messages
- **Per-peer tabs** - Individual conversation tabs for each peer
- **Active peers list** - Shows online/offline status of discovered peers
- **WhatsApp-inspired message bubbles** - Sent (blue) and received (gray) messages

## Technical Stack

- **Language**: C++ (C++17)
- **Framework**: Qt6 (with Qt5 fallback support)
- **Build System**: CMake
- **Network Protocol**: UDP (QUdpSocket)
- **Message Format**: JSON (QVariantMap serialization)

## Project Structure

```
02_Pawar_Rajni_PA2/
├── CMakeLists.txt           # CMake build configuration
├── README.md                # This file
├── src/                     # Source files
│   ├── main.cpp            # Application entry point
│   ├── simplechat.h/cpp    # Main controller
│   ├── chatwindow.h/cpp    # GUI implementation
│   ├── networkmanager.h/cpp # UDP networking and protocols
│   └── message.h/cpp       # Message data structure
├── scripts/                 # Helper scripts
│   ├── build.sh            # Build script
│   ├── launch_2_nodes.sh   # Launch 2 nodes for testing
│   ├── launch_4_nodes.sh   # Launch 4 nodes for testing
│   └── stop_all.sh         # Stop all running instances
└── tests/                   # Test cases (optional)
```

## Prerequisites

### macOS
```bash
# Install Qt6 using Homebrew
brew install qt@6

# Install CMake
brew install cmake
```

### Linux (Ubuntu/Debian)
```bash
# Install Qt6
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev cmake build-essential

# Or for Qt5
sudo apt-get install qtbase5-dev qttools5-dev cmake build-essential
```

### Environment Setup
Make sure Qt is in your PATH:
```bash
# For Qt6 on macOS
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"

# Verify Qt installation
qmake --version
```

## Building the Project

### Using the Build Script (Recommended)
```bash
cd 02_Pawar_Rajni_PA2
./scripts/build.sh
```

### Manual Build
```bash
cd 02_Pawar_Rajni_PA2
mkdir build
cd build
cmake ..
make -j$(nproc)
```

The executable will be located at: `build/SimpleChat_P2P`

## Running the Application

### Option 1: Using Launch Scripts (Recommended)

**Launch 2 nodes:**
```bash
./scripts/launch_2_nodes.sh
```

**Launch 4 nodes:**
```bash
./scripts/launch_4_nodes.sh
```

**Stopping nodes:**
- **Press Ctrl+C** in the terminal running the launch script to stop all nodes
- Or close individual GUI windows (each window quits its own process)
- Or use the stop script: `./scripts/stop_all.sh`
- Or manually kill processes: `pkill SimpleChat_P2P`

### Option 2: Manual Launch (Foreground)

**Launch individual nodes:**
```bash
# Terminal 1 - Node 1
./build/SimpleChat_P2P -p 9001 --peers 9001,9002,9003,9004

# Terminal 2 - Node 2
./build/SimpleChat_P2P -p 9002 --peers 9001,9002,9003,9004

# Terminal 3 - Node 3
./build/SimpleChat_P2P -p 9003 --peers 9001,9002,9003,9004

# Terminal 4 - Node 4
./build/SimpleChat_P2P -p 9004 --peers 9001,9002,9003,9004
```

### Command Line Options
- `-p, --port <port>` : Port number for this node (default: 9001)
- `--peers <ports>` : Comma-separated list of peer ports for discovery
- `-h, --help` : Display help information
- `-v, --version` : Display version information

## Testing Instructions

### Test 1: Basic P2P Messaging
1. Launch 2 nodes using `./scripts/launch_2_nodes.sh`
2. Wait for peer discovery (check System tab)
3. On Node1, select "Node2" from dropdown and type a message
4. Click "Send" or press Enter
5. Verify message appears in Node2's window
6. Reply from Node2 to Node1
7. Verify bidirectional communication works

### Test 2: Broadcast Messaging
1. Launch 4 nodes using `./scripts/launch_4_nodes.sh`
2. Wait for all peers to discover each other
3. On any node, click the "Broadcast" tab
4. Type a message and click "Broadcast"
5. Verify the message appears in the Broadcast tab of all other nodes
6. Test from multiple nodes to verify broadcast functionality

### Test 3: Message Ordering
1. Launch 2 nodes
2. Send multiple messages quickly from Node1 to Node2
3. Verify messages appear in the correct order on Node2
4. Sequence numbers should be sequential (1, 2, 3, ...)

### Test 4: Anti-Entropy Synchronization
1. Launch 3 nodes (Node1, Node2, Node3)
2. Stop Node3 temporarily (or start it later)
3. Send messages between Node1 and Node2
4. Start Node3 (or restart it)
5. Wait for anti-entropy to trigger (2-3 seconds)
6. Verify Node3 receives all messages it missed
7. Check System tab for anti-entropy sync messages

### Test 5: Reliable Delivery with Retries
1. Launch 2 nodes
2. Send a message from Node1 to Node2
3. Quickly stop Node2 before ACK is sent
4. Observe retry attempts in Node1's debug output
5. Restart Node2
6. Verify message is delivered after retry

### Test 6: Peer Discovery
1. Launch Node1 and Node2
2. Verify they discover each other (check Active Peers list)
3. Launch Node3 and Node4
4. Verify all nodes discover each other
5. Stop a node and verify others detect it as offline after timeout

### Test 7: Multi-hop Message Propagation
1. Launch 3 nodes
2. Ensure all are connected
3. Send message from Node1 to Node3
4. Message should reach Node3 directly (P2P)
5. With anti-entropy, verify message history propagates through Node2

## Implementation Details

### UDP vs TCP
This implementation uses UDP instead of TCP for several reasons:
- Faster message delivery (no connection setup overhead)
- Better suited for broadcast messaging
- Allows for custom reliability mechanisms (ACK/retry)
- More suitable for peer-to-peer gossip protocols

### Vector Clock Algorithm
Each node maintains a vector clock as a map: `{origin -> max_sequence_number}`

**Example:**
```
Node1 vector clock: {Node1: 5, Node2: 3, Node3: 7}
Node2 vector clock: {Node1: 3, Node2: 4, Node3: 7}
```

When Node1 and Node2 exchange vector clocks:
- Node2 sees Node1 has messages from Node1 up to seq 5, but Node2 only has up to seq 3
- Node2 requests messages Node1_4 and Node1_5 from Node1
- Node1 syncs these missing messages to Node2

### Retry Mechanism
- Messages requiring ACK are stored in `pendingAcks` map
- ACK timeout: 2 seconds
- Max retries: 3
- On timeout, message is retransmitted
- On receiving ACK, message is removed from pending queue

### Message Types
1. **CHAT_MESSAGE**: Regular P2P or broadcast chat messages
2. **ANTI_ENTROPY_REQUEST**: Request for missing messages with vector clock
3. **ANTI_ENTROPY_RESPONSE**: Response with vector clock
4. **ACK**: Acknowledgment of received chat message

## Known Limitations

1. **Local network only**: Currently configured for localhost testing
2. **No encryption**: Messages are sent in plaintext
3. **No persistent storage**: Messages are lost when application closes
4. **Limited scalability**: Tested with up to 4 nodes
5. **Fixed ports**: Uses predefined port range (9001-9004)

## Future Enhancements

- [ ] Add support for remote IP addresses
- [ ] Implement message encryption (TLS/SSL)
- [ ] Add persistent message storage (SQLite)
- [ ] Support for file transfers
- [ ] Improve UI with message timestamps
- [ ] Add user authentication
- [ ] Implement NAT traversal for WAN communication
- [ ] Add message delivery confirmation UI

## Troubleshooting

### Build Issues

**Qt not found:**
```bash
# Set Qt path
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"
# or
export CMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake/Qt6"
```

**Compiler errors:**
- Ensure C++17 support: `g++ --version` or `clang++ --version`
- Update CMake: `brew upgrade cmake` or `sudo apt-get upgrade cmake`

### Runtime Issues

**Port already in use:**
```bash
# Find process using port
lsof -i :9001
# Kill process
kill -9 <PID>
# Or use stop script
./scripts/stop_all.sh
```

**Peers not discovering:**
- Check firewall settings
- Verify all nodes use same peer list
- Check System tab for discovery messages
- Ensure ports are not blocked

**Messages not being delivered:**
- Check both nodes are online (Active Peers list)
- Verify sequence numbers in debug output
- Check for network errors in System tab
- Try restarting nodes

### Debug Output

To see detailed debug information, run with Qt debug output:
```bash
QT_LOGGING_RULES="*.debug=true" ./build/SimpleChat_P2P -p 9001
```

## Submission Checklist

- [x] Source code with proper structure
- [x] CMakeLists.txt for building
- [x] README with comprehensive instructions
- [x] Build script
- [x] Launch scripts for testing
- [x] UDP-based implementation
- [x] Broadcast support
- [x] Anti-entropy protocol
- [x] Peer discovery
- [x] Message retry mechanism
- [x] Vector clock implementation
- [x] Proper sequence numbering
- [ ] Git repository with commit history

## References

- Qt6 Documentation: https://doc.qt.io/qt-6/
- UDP Socket Programming: https://doc.qt.io/qt-6/qudpsocket.html
- Vector Clocks: Leslie Lamport, "Time, Clocks, and the Ordering of Events"
- Anti-Entropy: "Epidemic Algorithms for Replicated Database Maintenance"

## Contact

For questions or issues:
- Student: Rajni Pawar
- Course: CS 550 - Advanced Operating Systems
- Semester: Fall 2025

---

**Note:** This implementation is for educational purposes as part of CS 550 coursework.
