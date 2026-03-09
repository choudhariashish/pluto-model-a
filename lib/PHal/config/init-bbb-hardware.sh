#!/bin/bash
# BeagleBone Black Hardware Initialization Script

# P8 Header:
# Row1  Pin      Function    GPIO#  PWM            Notes              ||  Row2  Pin      Function    GPIO#  PWM            Notes
# ======================================================================================================================================================
# 1     P8.1     GND         -      -              Ground              ||  2     P8.2     GND         -      -              Ground
# 3     P8.3     GPIO        38     -              GPIO1_6             ||  4     P8.4     GPIO        39     -              GPIO1_7
# 5     P8.5     GPIO        34     -              GPIO1_2             ||  6     P8.6     GPIO        35     -              GPIO1_3
# 7     P8.7     GPIO        66     -              GPIO2_2             ||  8     P8.8     GPIO        67     -              GPIO2_3
# 9     P8.9     GPIO        69     -              GPIO2_5             ||  10    P8.10    GPIO        68     -              GPIO2_4
# 11    P8.11    GPIO        45     -              GPIO1_13            ||  12    P8.12    GPIO        44     -              GPIO1_12
# 13    P8.13    PWM/GPIO    23     pwmchip2/1     ehrpwm2_b           ||  14    P8.14    GPIO        26     -              GPIO0_26
# 15    P8.15    GPIO        47     -              GPIO1_15            ||  16    P8.16    GPIO        46     -              GPIO1_14
# 17    P8.17    GPIO        27     -              GPIO0_27            ||  18    P8.18    GPIO        65     -              GPIO2_1
# 19    P8.19    PWM/GPIO    22     pwmchip2/0     ehrpwm2_a           ||  20    P8.20    GPIO        63     -              GPIO1_31
# 21    P8.21    GPIO        62     -              GPIO1_30            ||  22    P8.22    GPIO        37     -              GPIO1_5
# 23    P8.23    GPIO        36     -              GPIO1_4             ||  24    P8.24    GPIO        33     -              GPIO1_1
# 25    P8.25    GPIO        32     -              GPIO1_0             ||  26    P8.26    GPIO        61     -              GPIO1_29
# 27    P8.27    GPIO        86     -              GPIO2_22            ||  28    P8.28    GPIO        88     -              GPIO2_24
# 29    P8.29    GPIO        87     -              GPIO2_23            ||  30    P8.30    GPIO        89     -              GPIO2_25
# 31    P8.31    GPIO        10     -              GPIO0_10            ||  32    P8.32    GPIO        11     -              GPIO0_11
# 33    P8.33    GPIO        9      -              GPIO0_9             ||  34    P8.34    GPIO        81     -              GPIO2_17
# 35    P8.35    GPIO        8      -              GPIO0_8             ||  36    P8.36    GPIO        80     -              GPIO2_16
# 37    P8.37    GPIO        78     -              GPIO2_14            ||  38    P8.38    GPIO        79     -              GPIO2_15
# 39    P8.39    GPIO        76     -              GPIO2_12            ||  40    P8.40    GPIO        77     -              GPIO2_13
# 41    P8.41    GPIO        74     -              GPIO2_10            ||  42    P8.42    GPIO        75     -              GPIO2_11
# 43    P8.43    GPIO        72     -              GPIO2_8             ||  44    P8.44    GPIO        73     -              GPIO2_9
# 45    P8.45    GPIO        70     -              GPIO2_6             ||  46    P8.46    GPIO        71     -              GPIO2_7
#
# P9 Header:
# Row1  Pin      Function    GPIO#  PWM            Notes              ||  Row2  Pin      Function    GPIO#  PWM            Notes
# ======================================================================================================================================================
# 1     P9.1     GND         -      -              Ground              ||  2     P9.2     GND         -      -              Ground
# 3     P9.3     VDD_3V3     -      -              3.3V Power          ||  4     P9.4     VDD_3V3     -      -              3.3V Power
# 5     P9.5     VDD_5V      -      -              5V Power            ||  6     P9.6     VDD_5V      -      -              5V Power
# 7     P9.7     SYS_5V      -      -              5V System           ||  8     P9.8     SYS_5V      -      -              5V System
# 9     P9.9     PWR_BUT     -      -              Power Button        ||  10    P9.10    SYS_RESET   -      -              System Reset
# 11    P9.11    GPIO        30     -              GPIO0_30            ||  12    P9.12    GPIO        60     -              GPIO1_28
# 13    P9.13    GPIO        31     -              GPIO0_31            ||  14    P9.14    PWM/GPIO    50     pwmchip1/0     ehrpwm1_a
# 15    P9.15    GPIO        48     -              GPIO1_16            ||  16    P9.16    PWM/GPIO    51     pwmchip1/1     ehrpwm1_b
# 17    P9.17    GPIO        5      -              GPIO0_5             ||  18    P9.18    GPIO        4      -              GPIO0_4
# 19    P9.19    I2C         -      -              I2C2_SCL            ||  20    P9.20    I2C         -      -              I2C2_SDA
# 21    P9.21    PWM/GPIO    3      pwmchip0/1     ehrpwm0_b           ||  22    P9.22    PWM/GPIO    2      pwmchip0/0     ehrpwm0_a
# 23    P9.23    GPIO        49     -              GPIO1_17            ||  24    P9.24    GPIO        15     -              GPIO0_15
# 25    P9.25    GPIO        117    -              GPIO3_21            ||  26    P9.26    GPIO        14     -              GPIO0_14
# 27    P9.27    GPIO        115    -              GPIO3_19            ||  28    P9.28    GPIO        113    -              GPIO3_17
# 29    P9.29    GPIO        111    -              GPIO3_15            ||  30    P9.30    GPIO        112    -              GPIO3_16
# 31    P9.31    GPIO        110    -              GPIO3_14            ||
#                                                                      ||
#                                                                      ||
#                                                                      ||
#                                                                      ||
# 41    P9.41    GPIO        20     -              GPIO0_20            ||  42    P9.42    GPIO        7      -              GPIO0_7



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
echo "PWM initialization complete"



# ====================================================================
# GPIO Initialization
# ====================================================================
echo "Exporting GPIOs..."

# P9.11 = GPIO30
echo 30 > /sys/class/gpio/export
sleep 0.1

# P8.14 = GPIO26
echo 26 > /sys/class/gpio/export
sleep 0.1

# P9.26 = GPIO14
echo 14 > /sys/class/gpio/export
sleep 0.1

# P8.12 = GPIO44
echo 44 > /sys/class/gpio/export
sleep 0.1

echo "GPIO initialization complete"
