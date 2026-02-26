#!/bin/bash
# Deploy script for BeagleBone AI-64
# Usage: ./deploy.sh [target] [host]

set -e

TARGET="${1:-//labs/ai64-demo:hello}"
HOST="${2:-debian@beaglebone.local}"
REMOTE_PATH="${3:-~/}"

# Extract binary path from target
# Convert //labs/ai64-demo:hello to bazel-bin/labs/ai64-demo/hello
TARGET_PATH="${TARGET#//}"  # Remove leading //
TARGET_PATH="${TARGET_PATH/://}"  # Replace : with /
BINARY_PATH="bazel-bin/${TARGET_PATH}"

echo "Building ARM64 binary: $TARGET"
bazel build --config=release-arm64 "$TARGET"

echo "Deploying to $HOST:$REMOTE_PATH"
scp "$BINARY_PATH" "$HOST:$REMOTE_PATH"

echo "✓ Deployed successfully!"
echo "To run: ssh $HOST './${BINARY_PATH##*/}'"
