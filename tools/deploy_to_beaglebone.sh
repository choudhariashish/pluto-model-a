#!/bin/bash
# Universal deploy script for any Bazel target to BeagleBone AI-64
# Usage: bazel run //tools:deploy -- <target> [host] [remote_path]

set -e

TARGET="${1}"
HOST="${2:-debian@beaglebone.local}"
REMOTE_PATH="${3:-~/}"

if [ -z "$TARGET" ]; then
    echo "Usage: bazel run //tools:deploy -- <target> [host] [remote_path]"
    echo "Example: bazel run //tools:deploy -- //labs/ai64-demo:hello"
    exit 1
fi

# Get workspace directory (set by bazel run)
WORKSPACE_DIR="${BUILD_WORKSPACE_DIRECTORY:-$(pwd)}"

# Build the target for ARM64
echo "Building ARM64 binary: $TARGET"
cd "$WORKSPACE_DIR"
bazel build --config=release-arm64 "$TARGET"

# Find the binary in bazel-bin
# Convert //labs/ai64-demo:hello to bazel-bin/labs/ai64-demo/hello
TARGET_PATH="${TARGET#//}"  # Remove leading //
TARGET_PATH="${TARGET_PATH/://}"  # Replace : with /
BINARY_PATH="$WORKSPACE_DIR/bazel-bin/${TARGET_PATH}"
BINARY_NAME="${BINARY_PATH##*/}"

if [ ! -f "$BINARY_PATH" ]; then
    echo "Error: Binary not found at $BINARY_PATH"
    exit 1
fi

echo "Deploying $BINARY_NAME to $HOST:$REMOTE_PATH"
scp "$BINARY_PATH" "$HOST:$REMOTE_PATH"

echo ""
echo "✓ Deployed successfully!"
echo "To run on BeagleBone:"
echo "  ssh $HOST"
echo "  ./$BINARY_NAME"
