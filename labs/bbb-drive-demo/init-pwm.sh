#!/bin/bash
# BeagleBone Black PWM Initialization Script
# This script loads PWM overlays for motor control on boot

# Find cape manager slots file
SLOTS=$(ls /sys/devices/bone_capemgr.*/slots 2>/dev/null | head -1)

if [ -z "$SLOTS" ]; then
    echo "Error: Cape manager not found"
    exit 1
fi

echo "Cape manager found at: $SLOTS"

# Load base PWM subsystem overlay first
echo "Loading base PWM subsystem (am33xx_pwm)..."
echo am33xx_pwm > $SLOTS 2>/dev/null || echo "am33xx_pwm already loaded or failed"
sleep 1

# Load PWM overlay for P9.14 (M1)
echo "Loading PWM overlay for P9.14..."
echo bone_pwm_P9_14 > $SLOTS 2>/dev/null || echo "P9.14 overlay already loaded or failed"

# Load PWM overlay for P9.21 (M2)
echo "Loading PWM overlay for P9.21..."
echo bone_pwm_P9_21 > $SLOTS 2>/dev/null || echo "P9.21 overlay already loaded or failed"

# Save configuration info
echo "Saving configuration..."
cat > /tmp/bbb-pwms.info << EOF
PWM Configuration:
- M1 PWM: P9.14 (pwmchip1, channel 0)
- M2 PWM: P9.21 (pwmchip0, channel 1)

GPIO Configuration:
- M1 Direction: P9.11 (GPIO30, output)
- M1 Tachometer: P8.14 (GPIO26, input, rising edge)
- M2 Direction: P9.26 (GPIO14, output)
- M2 Tachometer: P8.12 (GPIO44, input, rising edge)

Loaded Overlays:
$(cat $SLOTS | grep pwm)
EOF

# Display loaded overlays
echo "Loaded PWM overlays:"
cat $SLOTS | grep pwm

echo "PWM initialization complete"

# Export GPIOs for motor control
echo "Exporting GPIOs..."

# M1 Direction: P9.11 = GPIO30
if [ ! -d /sys/class/gpio/gpio30 ]; then
    echo 30 > /sys/class/gpio/export 2>/dev/null || echo "GPIO30 already exported"
    sleep 0.1
    echo out > /sys/class/gpio/gpio30/direction 2>/dev/null
fi

# M1 Tachometer: P8.14 = GPIO26
if [ ! -d /sys/class/gpio/gpio26 ]; then
    echo 26 > /sys/class/gpio/export 2>/dev/null || echo "GPIO26 already exported"
    sleep 0.1
    echo in > /sys/class/gpio/gpio26/direction 2>/dev/null
    echo rising > /sys/class/gpio/gpio26/edge 2>/dev/null
fi

# M2 Direction: P9.26 = GPIO14
if [ ! -d /sys/class/gpio/gpio14 ]; then
    echo 14 > /sys/class/gpio/export 2>/dev/null || echo "GPIO14 already exported"
    sleep 0.1
    echo out > /sys/class/gpio/gpio14/direction 2>/dev/null
fi

# M2 Tachometer: P8.12 = GPIO44
if [ ! -d /sys/class/gpio/gpio44 ]; then
    echo 44 > /sys/class/gpio/export 2>/dev/null || echo "GPIO44 already exported"
    sleep 0.1
    echo in > /sys/class/gpio/gpio44/direction 2>/dev/null
    echo rising > /sys/class/gpio/gpio44/edge 2>/dev/null
fi

echo "GPIO initialization complete"
