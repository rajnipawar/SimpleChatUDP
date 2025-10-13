#!/bin/bash

# Script to launch 2 SimpleChat P2P nodes for testing
# Usage: ./launch_2_nodes.sh

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

echo "Launching 2 SimpleChat P2P nodes..."
echo "Ports: 9001, 9002"
echo ""

# Launch nodes with peer discovery
"$BUILD_DIR/SimpleChat_P2P" -p 9001 --peers 9001,9002 &
sleep 1

"$BUILD_DIR/SimpleChat_P2P" -p 9002 --peers 9001,9002 &

echo ""
echo "All nodes launched!"
echo "To stop all nodes, run: ./scripts/stop_all.sh"
