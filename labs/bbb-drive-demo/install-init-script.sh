#!/bin/bash
# Install PWM initialization script on BeagleBone Black
# This script copies the init script and configures it to run at boot

echo "Installing PWM initialization script..."

# Copy init script to /usr/local/bin
sudo cp init-pwm.sh /usr/local/bin/init-pwm.sh
sudo chmod +x /usr/local/bin/init-pwm.sh

echo "Init script installed to /usr/local/bin/init-pwm.sh"

# Create systemd service file
cat << 'EOF' | sudo tee /etc/systemd/system/bbb-pwm-init.service
[Unit]
Description=BeagleBone Black PWM Initialization
After=multi-user.target
Before=network.target

[Service]
Type=oneshot
ExecStart=/usr/local/bin/init-pwm.sh
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

echo "Systemd service file created at /etc/systemd/system/bbb-pwm-init.service"

# Enable the service to run at boot
sudo systemctl daemon-reload
sudo systemctl enable bbb-pwm-init.service

echo "Service enabled to run at boot"
echo ""
echo "To test the service now, run:"
echo "  sudo systemctl start bbb-pwm-init.service"
echo "  sudo systemctl status bbb-pwm-init.service"
echo ""
echo "To view logs:"
echo "  journalctl -u bbb-pwm-init.service"
