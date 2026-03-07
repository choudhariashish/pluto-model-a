#!/bin/bash
# BeagleBone Black PWM Initialization Script

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

# Load PWM overlays
echo "Loading PWM overlay for P9.14..."
echo bone_pwm_P9_14 > $SLOTS 2>/dev/null || echo "P9.14 overlay already loaded or failed"

echo "Loading PWM overlay for P9.16..."
echo bone_pwm_P9_16 > $SLOTS 2>/dev/null || echo "P9.16 overlay already loaded or failed"

echo "Loading PWM overlay for P9.21..."
echo bone_pwm_P9_21 > $SLOTS 2>/dev/null || echo "P9.21 overlay already loaded or failed"

echo "Loading PWM overlay for P9.22..."
echo bone_pwm_P9_22 > $SLOTS 2>/dev/null || echo "P9.22 overlay already loaded or failed"

echo "Loading PWM overlay for P8.19..."
echo bone_pwm_P8_19 > $SLOTS 2>/dev/null || echo "P8.19 overlay already loaded or failed"

sleep 1

# Configure PWM outputs (2 kHz, enabled by default)
echo "Configuring PWM outputs to 2 kHz..."

# P9.14 - EHRPWM1A
if [ -d /sys/devices/ocp.3/pwm_test_P9_14.15 ]; then
    echo 500000 > /sys/devices/ocp.3/pwm_test_P9_14.15/period
    echo 0 > /sys/devices/ocp.3/pwm_test_P9_14.15/duty
    echo 1 > /sys/devices/ocp.3/pwm_test_P9_14.15/run
    echo "P9.14 configured (period: $(cat /sys/devices/ocp.3/pwm_test_P9_14.15/period) ns)"
fi

# P9.16 - EHRPWM1B
if [ -d /sys/devices/ocp.3/pwm_test_P9_16.17 ]; then
    echo 500000 > /sys/devices/ocp.3/pwm_test_P9_16.17/period
    echo 0 > /sys/devices/ocp.3/pwm_test_P9_16.17/duty
    echo 1 > /sys/devices/ocp.3/pwm_test_P9_16.17/run
    echo "P9.16 configured (period: $(cat /sys/devices/ocp.3/pwm_test_P9_16.17/period) ns)"
fi

# P9.21 - EHRPWM0B
if [ -d /sys/devices/ocp.3/pwm_test_P9_21.16 ]; then
    echo 500000 > /sys/devices/ocp.3/pwm_test_P9_21.16/period
    echo 0 > /sys/devices/ocp.3/pwm_test_P9_21.16/duty
    echo 1 > /sys/devices/ocp.3/pwm_test_P9_21.16/run
    echo "P9.21 configured (period: $(cat /sys/devices/ocp.3/pwm_test_P9_21.16/period) ns)"
fi

# P9.22 - EHRPWM0A
if [ -d /sys/devices/ocp.3/pwm_test_P9_22.18 ]; then
    echo 500000 > /sys/devices/ocp.3/pwm_test_P9_22.18/period
    echo 0 > /sys/devices/ocp.3/pwm_test_P9_22.18/duty
    echo 1 > /sys/devices/ocp.3/pwm_test_P9_22.18/run
    echo "P9.22 configured (period: $(cat /sys/devices/ocp.3/pwm_test_P9_22.18/period) ns)"
fi

# P8.19 - EHRPWM2A
if [ -d /sys/devices/ocp.3/pwm_test_P8_19.* ]; then
    echo 500000 > /sys/devices/ocp.3/pwm_test_P8_19.*/period
    echo 0 > /sys/devices/ocp.3/pwm_test_P8_19.*/duty
    echo 1 > /sys/devices/ocp.3/pwm_test_P8_19.*/run
    echo "P8.19 configured (period: $(cat /sys/devices/ocp.3/pwm_test_P8_19.*/period) ns)"
fi

# Save configuration info
echo "Saving configuration..."
cat > /tmp/bbb-pwms.info << EOF
PWM Configuration (2 kHz / 500000 ns period):
- P9.14 (EHRPWM1A): /sys/devices/ocp.3/pwm_test_P9_14.15/
- P9.16 (EHRPWM1B): /sys/devices/ocp.3/pwm_test_P9_16.17/
- P9.21 (EHRPWM0B): /sys/devices/ocp.3/pwm_test_P9_21.16/
- P9.22 (EHRPWM0A): /sys/devices/ocp.3/pwm_test_P9_22.18/
- P8.19 (EHRPWM2A): /sys/devices/ocp.3/pwm_test_P8_19.*/

Note: P8.13 (EHRPWM2B) cannot be used simultaneously with P8.19 (same EHRPWM2 module)

Loaded Overlays:
$(cat $SLOTS | grep pwm)
EOF

# Display loaded overlays
echo "Loaded PWM overlays:"
cat $SLOTS | grep pwm

echo "PWM initialization complete"




# Export GPIOs for motor control
echo "Exporting GPIOs..."

# P9.11 = GPIO30
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
