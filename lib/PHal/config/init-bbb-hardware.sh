#!/bin/bash
# BeagleBone Black Hardware Initialization Script

# P8 Header:
# Row1        Pin     Function   GPIO#  ||  Row2       Pin     Function   GPIO#
# =============================================================================
# 1     1     P8.1    GND        -      ||  1    2     P8.2    GND        -
# 2     3     P8.3    GPIO       38     ||  2    4     P8.4    GPIO       39
# 3     5     P8.5    GPIO       34     ||  3    6     P8.6    GPIO       35
# 4     7     P8.7    GPIO       66     ||  4    8     P8.8    GPIO       67
# 5     9     P8.9    GPIO       69     ||  5    10    P8.10   GPIO       68
# 6     11    P8.11   GPIO       45     ||  6    12    P8.12   GPIO       44
# 7     13    P8.13   PWM/GPIO   23     ||  7    14    P8.14   GPIO       26
# 8     15    P8.15   GPIO       47     ||  8    16    P8.16   GPIO       46
# 9     17    P8.17   GPIO       27     ||  9    18    P8.18   GPIO       65
# 10    19    P8.19   PWM/GPIO   22     ||  10   20    P8.20   GPIO       63
# 11    21    P8.21   GPIO       62     ||  11   22    P8.22   GPIO       37
# 12    23    P8.23   GPIO       36     ||  12   24    P8.24   GPIO       33
# 13    25    P8.25   GPIO       32     ||  13   26    P8.26   GPIO       61
# 14    27    P8.27   GPIO       86     ||  14   28    P8.28   GPIO       88
# 15    29    P8.29   GPIO       87     ||  15   30    P8.30   GPIO       89
# 16    31    P8.31   GPIO       10     ||  16   32    P8.32   GPIO       11
# 17    33    P8.33   GPIO       9      ||  17   34    P8.34   GPIO       81
# 18    35    P8.35   GPIO       8      ||  18   36    P8.36   GPIO       80
# 19    37    P8.37   GPIO       78     ||  19   38    P8.38   GPIO       79
# 20    39    P8.39   GPIO       76     ||  20   40    P8.40   GPIO       77
# 21    41    P8.41   GPIO       74     ||  21   42    P8.42   GPIO       75
# 22    43    P8.43   GPIO       72     ||  22   44    P8.44   GPIO       73
# 23    45    P8.45   GPIO       70     ||  23   46    P8.46   GPIO       71
#
# P9 Header:
# Row1        Pin     Function   GPIO#  ||  Row2       Pin     Function    GPIO#
# ==============================================================================
# 1     1     P9.1    GND        -      ||  1    2     P9.2    GND         -
# 2     3     P9.3    VDD_3V3    -      ||  2    4     P9.4    VDD_3V3     -
# 3     5     P9.5    VDD_5V     -      ||  3    6     P9.6    VDD_5V      -
# 4     7     P9.7    SYS_5V     -      ||  4    8     P9.8    SYS_5V      -
# 5     9     P9.9    PWR_BUT    -      ||  5    10    P9.10   SYS_RESET   -
# 6     11    P9.11   GPIO       30     ||  6    12    P9.12   GPIO        60
# 7     13    P9.13   GPIO       31     ||  7    14    P9.14   PWM/GPIO    50
# 8     15    P9.15   GPIO       48     ||  8    16    P9.16   PWM/GPIO    51
# 9     17    P9.17   GPIO       5      ||  9    18    P9.18   GPIO        4
# 10    19    P9.19   I2C        -      ||  10   20    P9.20   I2C         -
# 11    21    P9.21   PWM/GPIO   3      ||  11   22    P9.22   PWM/GPIO    2
# 12    23    P9.23   GPIO       49     ||  12   24    P9.24   GPIO        15
# 13    25    P9.25   GPIO       117    ||  13   26    P9.26   GPIO        14
# 14    27    P9.27   GPIO       115    ||  14   28    P9.28   GPIO        113
# 15    29    P9.29   GPIO       111    ||  15   30    P9.30   GPIO        112
# 16    31    P9.31   GPIO       110    ||  16   32    P9.32   GPIO        114
# 17    33                              ||  17   34
# 18    35                              ||  18   36
# 19    37                              ||  19   38
# 20    39                              ||  20   40
# 21    41    P9.41   GPIO       20     ||  21   42    P9.42   GPIO        7
# 22    43                              ||  22   44
# 23    45                              ||  23   46


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

echo 30 > /sys/class/gpio/export # P9.11 = GPIO30
echo 26 > /sys/class/gpio/export # P8.14 = GPIO26
echo 14 > /sys/class/gpio/export # P9.26 = GPIO14
echo 44 > /sys/class/gpio/export # P8.12 = GPIO44
echo 115 > /sys/class/gpio/export # P9.27 = GPIO115
echo 112 > /sys/class/gpio/export # P9.30 = GPIO112
echo 66 > /sys/class/gpio/export # P8.7 = GPIO66
echo 67 > /sys/class/gpio/export # P8.8 = GPIO67
echo 69 > /sys/class/gpio/export # P8.9 = GPIO69
echo 68 > /sys/class/gpio/export # P8.10 = GPIO68
echo 45 > /sys/class/gpio/export # P8.11 = GPIO45
echo 47 > /sys/class/gpio/export # P8.15 = GPIO47
echo 46 > /sys/class/gpio/export # P8.16 = GPIO46
echo 27 > /sys/class/gpio/export # P8.17 = GPIO27
echo 65 > /sys/class/gpio/export # P8.18 = GPIO65
sleep 0.1

echo "GPIO initialization complete"
