#!/bin/bash

# Script to stop all running SimpleChat P2P instances
# Usage: ./stop_all.sh

echo "Stopping all SimpleChat_P2P instances..."

# Find and kill all SimpleChat_P2P processes
pkill -f "SimpleChat_P2P"

# Wait a moment
sleep 1

# Check if any processes are still running
REMAINING=$(pgrep -f "SimpleChat_P2P" | wc -l)

if [ "$REMAINING" -eq 0 ]; then
    echo "All SimpleChat_P2P instances stopped successfully."
else
    echo "Warning: $REMAINING processes still running. Force killing..."
    pkill -9 -f "SimpleChat_P2P"
    echo "All processes terminated."
fi
