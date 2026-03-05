# NVIDIA Orin Nano GPIO Demo

This demo shows how to use GPIOs on the NVIDIA Jetson Orin Nano platform using the `HalGpio` class from the PHal library.

## Hardware Platform

- **Device:** NVIDIA Jetson Orin Nano Developer Kit Super
- **JetPack:** 6.2.1 (R36.4.4)
- **Kernel:** 5.15.148-tegra
- **GPIO Library:** libgpiod

## GPIO Architecture on Orin Nano

The Orin Nano has two GPIO controllers:
- **gpiochip0** (tegra234-gpio): Main GPIO controller, base 348, lines 348-511
- **gpiochip1** (tegra234-gpio-aon): AON GPIO controller, base 316, lines 316-347

### GPIO Number Calculation

GPIO number = base + port_offset + pin_offset

Example for GPIO3_PH.00:
- base = 348 (from `dmesg | grep gpiochip`)
- port_offset = 43 (PORT_H from tegra234 port table)
- pin_offset = 0
- **GPIO number = 348 + 43 + 0 = 391**

### Common GPIO Pins on 40-Pin Header

| Pin Name | GPIO Port | Line Number | Physical Pin |
|----------|-----------|-------------|--------------|
| GPIO09   | PAC.06    | 106         | Pin 7        |
| GPIO11   | PAC.04    | 112         | Pin 13       |
| GPIO12   | PH.00     | 391         | Pin 16       |
| GPIO13   | PH.01     | 392         | Pin 18       |

Use `gpioinfo` on the Orin Nano to see all available GPIO lines.

## Applications

### 1. gpio_demo

Interactive demo that continuously toggles an output GPIO and reads an input GPIO.

**Features:**
- Toggles output GPIO every second
- Reads input GPIO continuously
- Clean shutdown with Ctrl+C

**Usage:**
```bash
sudo ./gpio_demo
```

**Default Configuration:**
- Output GPIO: gpiochip0 line 106 (GPIO09/PAC.06)
- Input GPIO: gpiochip0 line 112 (GPIO11/PAC.04)

### 2. gpio_simple

Command-line utility for simple GPIO operations with built-in 40-pin connector reference.

**Usage:**
```bash
# Show help and 40-pin GPIO mapping
sudo ./gpio_simple --help

# Read a GPIO
sudo ./gpio_simple read gpiochip0 106

# Set GPIO high
sudo ./gpio_simple write-high gpiochip0 106

# Set GPIO low
sudo ./gpio_simple write-low gpiochip0 106
```

**40-Pin Connector Reference:**

The `--help` option displays a complete mapping of the 40-pin connector showing:
- Physical pin numbers
- GPIO names (GPIO01, GPIO07, etc.)
- GPIO chip (gpiochip0/gpiochip1)
- Line numbers for libgpiod
- GPIO port names (PAC.06, PCC.04, etc.)
- Special function pins (I2C, SPI, UART, Power, Ground)

This makes it easy to identify which GPIO line number to use for any physical pin on the connector.

## Building

### Build for Orin Nano (ARM64)

The Orin Nano uses a dedicated toolchain (ARM GNU Toolchain 11.3) compatible with Ubuntu 22.04's GLIBC 2.35.

```bash
# Debug build
bazel build --config=orin-nano //labs/orin-gpio-demo:gpio_demo
bazel build --config=orin-nano //labs/orin-gpio-demo:gpio_simple

# Optimized build (recommended)
bazel build --config=release-orin-nano //labs/orin-gpio-demo:gpio_demo
bazel build --config=release-orin-nano //labs/orin-gpio-demo:gpio_simple
```

### Deploy to Orin Nano

```bash
# Copy binaries to Orin Nano
scp bazel-bin/labs/orin-gpio-demo/gpio_demo pluto@brain:~/
scp bazel-bin/labs/orin-gpio-demo/gpio_simple pluto@brain:~/

# SSH to Orin Nano and run
ssh pluto@brain
sudo ./gpio_demo
```

## Finding GPIO Numbers

On the Orin Nano, use these commands to identify GPIO lines:

```bash
# List all GPIO chips and lines
gpioinfo

# Find specific GPIO by name
gpioinfo | grep PAC.06

# Check kernel GPIO allocation
dmesg | grep gpiochip

# Debug GPIO status
sudo cat /sys/kernel/debug/gpio
```

## HalGpio API Usage

### Basic Pattern

```cpp
#include "PHal.h"

// Create GPIO object
HalGpio gpio("my_gpio");

// Configure settings
gpio.getSettings()->mode = PObject::State_t::PL_OUTPUT;  // or PL_INPUT
gpio.getSettings()->gpioChipName = "gpiochip0";
gpio.getSettings()->lineNum = 106;

// Initialize
if (gpio.initialize() != PObject::Status_t::PL_OK) {
    // Handle error
}

// Use GPIO
gpio.write(1);  // For output
int value = gpio.read();  // For input

// Cleanup
gpio.shutdown();
```

### GPIO Modes

- **PL_OUTPUT**: Configure as output for writing
- **PL_INPUT**: Configure as input for reading
- **PL_RISING_EVENT_COUNT**: Configure for edge detection (event counting)

## Important Notes

1. **Root Permissions**: GPIO operations require root/sudo access
2. **Pinmux Configuration**: Ensure pins are configured correctly in device tree
3. **40-Pin Header**: Use Jetson-IO tool for dynamic pin configuration
4. **libgpiod**: JetPack 6+ uses libgpiod (sysfs is deprecated)
5. **Line Numbers**: Verify GPIO line numbers with `gpioinfo` before use

## Troubleshooting

### "Cannot open gpio file"
- Check if gpiochip exists: `ls /dev/gpiochip*`
- Verify permissions: Run with `sudo`

### "Request line failed"
- GPIO may be in use by another process
- Check pinmux configuration
- Verify line number with `gpioinfo`

### Wrong GPIO Behavior
- Confirm correct GPIO number calculation
- Check physical wiring
- Verify pinmux settings match your hardware revision (A01/A03)

## References

- [NVIDIA Jetson Orin Documentation](https://docs.nvidia.com/jetson/)
- [libgpiod Documentation](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/about/)
- [Jetson GPIO Python Library](https://github.com/NVIDIA/jetson-gpio)
