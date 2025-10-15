#!/bin/bash

# Script to build and run unit tests
# Usage: ./scripts/run_tests.sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
BUILD_DIR="$PROJECT_DIR/build"

echo "==============================================="
echo "SimpleChat P2P - Unit Test Runner"
echo "==============================================="
echo ""

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with tests enabled
echo "Configuring CMake with tests enabled..."
cmake .. -DBUILD_TESTS=ON

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

# Build tests
echo ""
echo "Building tests..."
make test_basic

if [ $? -ne 0 ]; then
    echo "ERROR: Test build failed"
    exit 1
fi

# Run tests
echo ""
echo "Running tests..."
echo "==============================================="
ctest --output-on-failure

if [ $? -eq 0 ]; then
    echo ""
    echo "==============================================="
    echo "✅ All tests passed!"
    echo "==============================================="
else
    echo ""
    echo "==============================================="
    echo "❌ Some tests failed!"
    echo "==============================================="
    exit 1
fi
