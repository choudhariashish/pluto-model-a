#!/bin/bash
# Deploy script for NVIDIA Orin Nano GPIO Demo
# This script handles:
#   1. Building the ARM64 binaries
#   2. Deploying the applications to Orin Nano
#
# Usage: ./deploy.sh [host]

set -e

HOST="${1:-root@brain}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "========================================="
echo "NVIDIA Orin Nano GPIO Demo Deployment"
echo "========================================="
echo ""

# Step 1: Build the ARM64 binaries
echo "Step 1/2: Building ARM64 binaries for Orin Nano..."
cd "$PROJECT_ROOT"
bazel build --config=release-orin-nano //labs/orin-gpio-demo:gpio_demo
bazel build --config=release-orin-nano //labs/orin-gpio-demo:gpio_simple
echo "✓ Build complete"
echo ""

# Step 2: Deploy applications
echo "Step 2/2: Deploying applications to $HOST..."
echo ""

# Copy files to Orin Nano
echo "Copying binaries..."
scp \
    "$PROJECT_ROOT/bazel-bin/labs/orin-gpio-demo/gpio_demo" \
    "$PROJECT_ROOT/bazel-bin/labs/orin-gpio-demo/gpio_simple" \
    "$HOST:/root/"

# Run setup on Orin Nano
echo "Installing applications..."
ssh "$HOST" << 'REMOTE_SCRIPT'
mv /root/gpio_demo /usr/local/bin/
mv /root/gpio_simple /usr/local/bin/
chmod +x /usr/local/bin/gpio_demo
chmod +x /usr/local/bin/gpio_simple
echo "✓ Applications installed to /usr/local/bin/"
REMOTE_SCRIPT

echo ""
echo "========================================="
echo "✓ Deployment Complete!"
echo "========================================="
echo ""
echo "Applications installed to /usr/local/bin/ on Orin Nano."
echo ""
echo "To run the applications:"
echo "  ssh $HOST"
echo "  gpio_demo"
echo "  gpio_simple --help"
echo "  gpio_simple read gpiochip0 106"
echo ""
echo "Note: Running as root, no sudo required."
echo ""
