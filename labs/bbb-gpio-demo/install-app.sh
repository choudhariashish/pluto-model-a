#!/bin/bash
# Remote installation script for bbb-gpio-demo on BeagleBone Black
# Usage: ./install-app.sh <username> <password> <ip_address>

set -e

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <username> <password> <ip_address>"
    echo "Example: $0 root root 192.168.55.102"
    exit 1
fi

USERNAME=$1
PASSWORD=$2
IP_ADDRESS=$3
BINARY_NAME="bbb-gpio-demo"
BINARY_INSTALL_PATH="/usr/local/bin"

echo "=== BBB GPIO Demo Remote Installation ==="
echo "Target: $USERNAME@$IP_ADDRESS"
echo "Binary will be installed to: $BINARY_INSTALL_PATH/$BINARY_NAME"
echo ""

# Check if sshpass is installed
if ! command -v sshpass &> /dev/null; then
    echo "Error: sshpass is not installed"
    echo "Install it with: sudo apt-get install sshpass"
    exit 1
fi

# Build the binary
echo "[1/3] Building binary with Bazel..."
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$WORKSPACE_ROOT"
bazel build //labs/bbb-gpio-demo:bbb-gpio-demo --config=bbb

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

BINARY_PATH="$WORKSPACE_ROOT/bazel-bin/labs/bbb-gpio-demo/bbb-gpio-demo"

if [ ! -f "$BINARY_PATH" ]; then
    echo "Error: Binary not found at $BINARY_PATH"
    exit 1
fi

echo "Binary built successfully: $BINARY_PATH"

# Copy binary
echo "[2/3] Copying binary to target..."
sshpass -p "$PASSWORD" scp -o StrictHostKeyChecking=no "$BINARY_PATH" "$USERNAME@$IP_ADDRESS:/tmp/$BINARY_NAME"

# Install binary
echo "[3/3] Installing binary..."
sshpass -p "$PASSWORD" ssh -o StrictHostKeyChecking=no "$USERNAME@$IP_ADDRESS" << EOF
chmod +x /tmp/$BINARY_NAME
echo "$PASSWORD" | sudo -S cp /tmp/$BINARY_NAME $BINARY_INSTALL_PATH/$BINARY_NAME
echo "$PASSWORD" | sudo -S chmod +x $BINARY_INSTALL_PATH/$BINARY_NAME
rm -f /tmp/$BINARY_NAME
EOF

echo ""
echo "=== Installation Complete ==="
echo "Binary installed to: $BINARY_INSTALL_PATH/$BINARY_NAME"
echo ""
echo "To run the application:"
echo "  ssh $USERNAME@$IP_ADDRESS"
echo "  $BINARY_NAME pinmap"
echo ""
