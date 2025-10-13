#!/bin/bash

# Script to launch 4 SimpleChat P2P nodes
# Usage: ./launch_4_nodes.sh

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
BUILD_DIR="$PROJECT_DIR/build"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Please build the project first."
    echo "Run: cd $PROJECT_DIR && mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Check if executable exists
if [ ! -f "$BUILD_DIR/SimpleChat_P2P" ]; then
    echo "SimpleChat_P2P executable not found. Please build the project first."
    exit 1
fi

echo "Launching 4 SimpleChat P2P nodes..."
echo "Ports: 9001, 9002, 9003, 9004"
echo "Press Ctrl+C to stop all nodes"
echo ""

# Array to track process IDs
PIDS=()

# Trap Ctrl+C to kill all child processes
trap 'echo ""; echo "Stopping all nodes..."; kill ${PIDS[@]} 2>/dev/null; exit 0' INT TERM

# Launch nodes with peer discovery
"$BUILD_DIR/SimpleChat_P2P" -p 9001 --peers 9001,9002,9003,9004 &
PIDS+=($!)
sleep 1

"$BUILD_DIR/SimpleChat_P2P" -p 9002 --peers 9001,9002,9003,9004 &
PIDS+=($!)
sleep 1

"$BUILD_DIR/SimpleChat_P2P" -p 9003 --peers 9001,9002,9003,9004 &
PIDS+=($!)
sleep 1

"$BUILD_DIR/SimpleChat_P2P" -p 9004 --peers 9001,9002,9003,9004 &
PIDS+=($!)

echo ""
echo "All nodes launched!"
echo "Press Ctrl+C to stop all nodes"

# Wait for all background processes
wait
