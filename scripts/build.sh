#!/bin/bash

# Script to build the SimpleChat P2P project
# Usage: ./build.sh

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
BUILD_DIR="$PROJECT_DIR/build"

echo "Building SimpleChat P2P..."
echo "Project directory: $PROJECT_DIR"
echo ""

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR"

# Run CMake
echo "Running CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo ""
echo "Building project..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Build successful!"
echo "Executable: $BUILD_DIR/SimpleChat_P2P"
echo ""
echo "To launch nodes, use:"
echo "  ./scripts/launch_2_nodes.sh  (for 2 nodes)"
echo "  ./scripts/launch_4_nodes.sh  (for 4 nodes)"
