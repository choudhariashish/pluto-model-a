#!/bin/bash
# Remote installation script for bbb-drive-demo on BeagleBone Black
# Usage: ./install-app.sh <username> <password> <ip_address>

set -e

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <username> <password> <ip_address>"
    echo "Example: $0 debian temppwd 192.168.7.2"
    exit 1
fi

USERNAME=$1
PASSWORD=$2
IP_ADDRESS=$3
REMOTE_TEMP_DIR="/tmp/bbb-drive-demo-install"
BINARY_NAME="bbb-drive-demo"
BINARY_INSTALL_PATH="/usr/local/bin"

echo "=== BBB Drive Demo Remote Installation ==="
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
echo "[1/5] Building binary with Bazel..."
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$WORKSPACE_ROOT"
bazel build //labs/bbb-drive-demo:bbb-drive-demo --config=bbb

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

BINARY_PATH="$WORKSPACE_ROOT/bazel-bin/labs/bbb-drive-demo/bbb-drive-demo"

if [ ! -f "$BINARY_PATH" ]; then
    echo "Error: Binary not found at $BINARY_PATH"
    exit 1
fi

echo "Binary built successfully: $BINARY_PATH"

# Create remote temporary directory
echo "[2/5] Creating temporary directory..."
sshpass -p "$PASSWORD" ssh -o StrictHostKeyChecking=no "$USERNAME@$IP_ADDRESS" "mkdir -p $REMOTE_TEMP_DIR"

# Copy binary
echo "[3/5] Copying binary to target..."
sshpass -p "$PASSWORD" scp -o StrictHostKeyChecking=no "$BINARY_PATH" "$USERNAME@$IP_ADDRESS:$REMOTE_TEMP_DIR/$BINARY_NAME"

# Copy init script
echo "[4/5] Copying initialization script..."
sshpass -p "$PASSWORD" scp -o StrictHostKeyChecking=no "$SCRIPT_DIR/init-bbb-drive-demo.sh" "$USERNAME@$IP_ADDRESS:$REMOTE_TEMP_DIR/"

# Install binary and init script
echo "[5/5] Installing binary and initialization script..."
sshpass -p "$PASSWORD" ssh -o StrictHostKeyChecking=no "$USERNAME@$IP_ADDRESS" << EOF
cd $REMOTE_TEMP_DIR
chmod +x $BINARY_NAME
chmod +x init-bbb-drive-demo.sh

# Install binary to /usr/local/bin
echo "$PASSWORD" | sudo -S cp $BINARY_NAME $BINARY_INSTALL_PATH/$BINARY_NAME
echo "$PASSWORD" | sudo -S chmod +x $BINARY_INSTALL_PATH/$BINARY_NAME

# Install init script
echo "$PASSWORD" | sudo -S cp init-bbb-drive-demo.sh /usr/local/bin/init-bbb-drive-demo.sh
echo "$PASSWORD" | sudo -S chmod +x /usr/local/bin/init-bbb-drive-demo.sh

# Create systemd service
echo "$PASSWORD" | sudo -S tee /etc/systemd/system/bbb-drive-demo-init.service > /dev/null << 'SERVICEEOF'
[Unit]
Description=BeagleBone Black Drive Demo Initialization
After=multi-user.target
Before=network.target

[Service]
Type=oneshot
ExecStart=/usr/local/bin/init-bbb-drive-demo.sh
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
SERVICEEOF

# Enable and start the service
echo "$PASSWORD" | sudo -S systemctl daemon-reload
echo "$PASSWORD" | sudo -S systemctl enable bbb-drive-demo-init.service
echo "$PASSWORD" | sudo -S systemctl start bbb-drive-demo-init.service

echo ""
echo "Service status:"
echo "$PASSWORD" | sudo -S systemctl status bbb-drive-demo-init.service --no-pager

# Clean up temporary directory
rm -rf $REMOTE_TEMP_DIR
EOF

echo ""
echo "=== Installation Complete ==="
echo "Binary installed to: $BINARY_INSTALL_PATH/$BINARY_NAME"
echo "Init script installed and enabled as systemd service"
echo ""
echo "To run the application:"
echo "  ssh $USERNAME@$IP_ADDRESS"
echo "  sudo $BINARY_NAME"
echo ""
echo "To check init service status:"
echo "  ssh $USERNAME@$IP_ADDRESS 'sudo systemctl status bbb-drive-demo-init.service'"
echo ""
echo "To view init service logs:"
echo "  ssh $USERNAME@$IP_ADDRESS 'sudo journalctl -u bbb-drive-demo-init.service'"
