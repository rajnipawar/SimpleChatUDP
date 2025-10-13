# Quick Start Guide - SimpleChat P2P

## Build the Project

```bash
cd 02_Pawar_Rajni_PA2
./scripts/build.sh
```

## Run 4 Nodes (Recommended)

```bash
./scripts/launch_4_nodes.sh
```

This will launch 4 SimpleChat instances on ports 9001-9004.

## Test P2P Messaging

1. **Wait for peer discovery** (2-3 seconds) - Check "System" tab for "Discovered peer" messages
2. **On Node1** - Select "Node2" from dropdown and send a message
3. **On Node2** - You should receive the message in the Node1 conversation tab
4. **Reply from Node2 to Node1** to verify bidirectional communication

## Test Broadcast

1. **On any node** - Click the "Broadcast" tab
2. **Type a message** and click "Broadcast" button
3. **Check all other nodes** - The message should appear in their Broadcast tabs

## Test Anti-Entropy

1. **Start Node1 and Node2** - Exchange some messages
2. **Start Node3** (late joiner)
3. **Wait 2-3 seconds** - Node3 will sync all messages via anti-entropy
4. **Check Node3** - It should have received all messages

## Stop All Nodes

```bash
./scripts/stop_all.sh
```

## Manual Launch

```bash
# Launch individual nodes
./build/SimpleChat_P2P -p 9001 --peers 9001,9002,9003,9004 &
./build/SimpleChat_P2P -p 9002 --peers 9001,9002,9003,9004 &
./build/SimpleChat_P2P -p 9003 --peers 9001,9002,9003,9004 &
./build/SimpleChat_P2P -p 9004 --peers 9001,9002,9003,9004 &
```

## Troubleshooting

**Build fails:**
```bash
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
./scripts/build.sh
```

**Ports in use:**
```bash
./scripts/stop_all.sh
```

**Peers not connecting:**
- Wait 2-3 seconds for peer discovery
- Check firewall is not blocking localhost connections
- Verify all nodes show "Discovered peer" messages in System tab

## Features to Test

- [x] P2P messaging between any two nodes
- [x] Broadcast to all nodes
- [x] Message ordering (sequential messages maintain order)
- [x] Anti-entropy sync (late joiners get all messages)
- [x] Retry on failure (temporary disconnections)
- [x] Peer status updates (online/offline)
- [x] Per-peer conversation tabs

## Debug Mode

For detailed logs:
```bash
QT_LOGGING_RULES="*.debug=true" ./build/SimpleChat_P2P -p 9001
```

Enjoy testing SimpleChat P2P!
