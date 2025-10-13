# Submission Checklist - CS 550 PA2

**Student:** Rajni Pawar  
**Assignment:** Programming Assignment 2  
**Due Date:** October 13, 2025  

## ✅ Core Requirements

### Network Communication
- ✅ Changed from TCP to UDP (QUdpSocket)
- ✅ Message serialization using QVariantMap
- ✅ Local port discovery implemented
- ✅ Message format includes: ChatText, Origin, Destination, SequenceNumber

### Messaging Protocol
- ✅ Direct peer-to-peer (no ring topology)
- ✅ Message retry with 1-2 second timeout using QTimer
- ✅ ACK/retry mechanism for reliable delivery
- ✅ Anti-entropy with vector clocks
- ✅ Broadcast messaging to all peers
- ✅ Proper message sequencing

### Peer Discovery
- ✅ Local port discovery implemented
- ✅ Dynamic peer addition support
- ✅ Peer health monitoring

### Technical Requirements
- ✅ C++ language (C++17)
- ✅ Qt6 framework
- ✅ CMake build system
- ✅ Proper documentation (README)
- ✅ Git version control
- ✅ Support for multiple local instances
- ✅ Launch scripts included

### Testing
- ✅ 2-node testing script
- ✅ 4-node testing script
- ✅ Message propagation verified
- ✅ Message ordering tested
- ✅ Peer discovery tested
- ✅ Anti-entropy tested

### Documentation
- ✅ README.md with comprehensive instructions
- ✅ Build instructions
- ✅ Testing instructions
- ✅ PROJECT_SUMMARY.md with implementation details
- ✅ QUICKSTART.md for easy testing

## 📁 Project Structure

```
02_Pawar_Rajni_PA2/
├── CMakeLists.txt              ✅
├── README.md                   ✅
├── PROJECT_SUMMARY.md          ✅
├── QUICKSTART.md               ✅
├── SUBMISSION_CHECKLIST.md     ✅
├── .gitignore                  ✅
├── src/                        ✅
│   ├── main.cpp
│   ├── simplechat.h/cpp
│   ├── chatwindow.h/cpp
│   ├── networkmanager.h/cpp
│   └── message.h/cpp
├── scripts/                    ✅
│   ├── build.sh
│   ├── launch_2_nodes.sh
│   ├── launch_4_nodes.sh
│   └── stop_all.sh
└── tests/                      ✅
    ├── CMakeLists.txt
    └── test_basic.cpp
```

## 📊 Project Statistics

- **Total Files:** 20
- **Lines of Code:** 1,491 (src only)
- **Git Commits:** 5
- **Build Status:** ✅ Success
- **Executable Size:** 779 KB

## 🚀 Build & Run Instructions

### Build
```bash
cd 02_Pawar_Rajni_PA2
./scripts/build.sh
```

### Launch 4 Nodes
```bash
./scripts/launch_4_nodes.sh
```

### Stop All
```bash
./scripts/stop_all.sh
```

## ✨ Key Features Implemented

### UDP Communication
- Replaced TCP with UDP sockets
- Custom datagram protocol
- Message serialization to JSON

### Broadcast
- Special destination "-1" or "broadcast"
- Sends to all active peers
- Dedicated UI tab for broadcasts

### Anti-Entropy
- Vector clock implementation
- Periodic sync (every 2 seconds)
- Automatic missing message propagation
- Ensures eventual consistency

### Reliability
- ACK/timeout mechanism
- Retry on failure (max 3 attempts)
- Message deduplication
- Sequence number ordering

### UI Enhancements
- Dark theme with modern styling
- Per-peer conversation tabs
- Active peers list
- Broadcast tab
- System log tab

## 🧪 Testing Performed

1. ✅ Basic P2P messaging (2 nodes)
2. ✅ Multi-node P2P (4 nodes)
3. ✅ Broadcast to all peers
4. ✅ Message ordering
5. ✅ Late joiner sync (anti-entropy)
6. ✅ Peer discovery
7. ✅ Retry on failure
8. ✅ Peer health monitoring

## 📦 Submission Files

All files are tracked in Git and ready for submission:

```bash
# Create submission zip
cd "02_Pawar_Rajni_PA2"
git archive --format=zip --output=../02_Pawar_Rajni_PA2.zip HEAD
```

Or manually:
```bash
cd ..
zip -r 02_Pawar_Rajni_PA2.zip 02_Pawar_Rajni_PA2 \
  -x "*/build/*" "*/.git/*" "*.o" "*.a"
```

## ✅ Final Verification

- [x] Code compiles without errors
- [x] All features implemented and tested
- [x] Documentation is complete
- [x] Git repository initialized with proper commits
- [x] Scripts are executable
- [x] README includes all required information
- [x] Project follows naming convention: 02_Pawar_Rajni_PA2

## 📝 Submission Notes

**What to submit on Canvas:**
- `02_Pawar_Rajni_PA2.zip` (created from git archive)

**What's included in the zip:**
- Complete source code
- CMakeLists.txt for building
- Documentation (README, PROJECT_SUMMARY, QUICKSTART)
- Launch and build scripts
- Basic unit tests
- Git repository history

**Build tested on:**
- macOS (Darwin 24.6.0, arm64)
- Qt 6.x
- CMake 3.16+
- Clang/GCC with C++17 support

---

**Prepared by:** Rajni Pawar  
**Date:** October 13, 2025  
**Assignment:** CS 550 Programming Assignment 2
