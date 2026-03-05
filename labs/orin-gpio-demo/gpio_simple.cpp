#include "PHal.h"
#include <iostream>
#include <iomanip>

void printHelp(const char* progName) {
    std::cout << "\n=== NVIDIA Orin Nano 40-Pin GPIO Reference ===\n\n";
    std::cout << "Usage: " << progName << " <mode> <chip> <line>\n";
    std::cout << "       " << progName << " --help | -h\n\n";
    std::cout << "Modes:\n";
    std::cout << "  read         - Read GPIO input value\n";
    std::cout << "  write-high   - Set GPIO output to HIGH (1)\n";
    std::cout << "  write-low    - Set GPIO output to LOW (0)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << progName << " write-high gpiochip0 106\n";
    std::cout << "  " << progName << " read gpiochip0 112\n\n";
    
    std::cout << "40-Pin Connector GPIO Mapping:\n";
    std::cout << "┌──────┬──────────────┬──────┬──────┬──────┬──────────────┬──────┬──────┐\n";
    std::cout << "│ Pin  │ Name         │ Chip │ Line │ Pin  │ Name         │ Chip │ Line │\n";
    std::cout << "├──────┼──────────────┼──────┼──────┼──────┼──────────────┼──────┼──────┤\n";
    std::cout << "│  1   │ 3.3V         │  -   │  -   │  2   │ 5V           │  -   │  -   │\n";
    std::cout << "│  3   │ I2C_SDA      │  -   │  -   │  4   │ 5V           │  -   │  -   │\n";
    std::cout << "│  5   │ I2C_SCL      │  -   │  -   │  6   │ GND          │  -   │  -   │\n";
    std::cout << "│  7   │ GPIO09       │ ch0  │ 106  │  8   │ UART_RXD     │  -   │  -   │\n";
    std::cout << "│  9   │ GND          │  -   │  -   │ 10   │ UART_TXD     │  -   │  -   │\n";
    std::cout << "│ 11   │ UART_RTS     │  -   │  -   │ 12   │ GPIO15       │  -   │  -   │\n";
    std::cout << "│ 13   │ GPIO11       │ ch0  │ 112  │ 14   │ GND          │  -   │  -   │\n";
    std::cout << "│ 15   │ GPIO12       │ ch0  │ 424  │ 16   │ GPIO18       │ ch0  │ 441  │\n";
    std::cout << "│ 17   │ 3.3V         │  -   │  -   │ 18   │ GPIO19       │ ch0  │ 464  │\n";
    std::cout << "│ 19   │ SPI_MOSI     │  -   │  -   │ 20   │ GND          │  -   │  -   │\n";
    std::cout << "│ 21   │ SPI_MISO     │  -   │  -   │ 22   │ GPIO21       │ ch0  │ 397  │\n";
    std::cout << "│ 23   │ SPI_CLK      │  -   │  -   │ 24   │ SPI_CS0      │ ch0  │ 389  │\n";
    std::cout << "│ 25   │ GND          │  -   │  -   │ 26   │ SPI_CS1      │ ch0  │ 388  │\n";
    std::cout << "│ 27   │ I2C_SDA_1    │  -   │  -   │ 28   │ I2C_SCL_1    │  -   │  -   │\n";
    std::cout << "│ 29   │ GPIO01       │ ch0  │ 422  │ 30   │ GND          │  -   │  -   │\n";
    std::cout << "│ 31   │ GPIO07       │ ch0  │ 417  │ 32   │ GPIO32       │ ch0  │ 351  │\n";
    std::cout << "│ 33   │ GPIO13       │ ch0  │ 392  │ 34   │ GND          │  -   │  -   │\n";
    std::cout << "│ 35   │ GPIO08       │ ch0  │ 353  │ 36   │ GPIO16       │ ch0  │ 395  │\n";
    std::cout << "│ 37   │ GPIO14       │ ch0  │ 344  │ 38   │ GPIO20       │ ch0  │ 398  │\n";
    std::cout << "│ 39   │ GND          │  -   │  -   │ 40   │ GPIO17       │ ch0  │ 352  │\n";
    std::cout << "└──────┴──────────────┴──────┴──────┴──────┴──────────────┴──────┴──────┘\n\n";
    std::cout << "Notes:\n";
    std::cout << "  - ch0 = gpiochip0 (main GPIO controller)\n";
    std::cout << "  - Line numbers are for libgpiod access\n";
    std::cout << "  - GPIO operations require sudo/root privileges\n";
    std::cout << "  - Use 'gpioinfo' command to see all available GPIOs\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
        printHelp(argv[0]);
        return (argc < 2) ? 1 : 0;
    }
    
    if (argc < 4) {
        std::cout << "Error: Missing arguments\n";
        printHelp(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    const char* chipName = argv[2];
    int lineNum = atoi(argv[3]);

    HalGpio gpio("gpio_simple");
    gpio.getSettings()->gpioChipName = chipName;
    gpio.getSettings()->lineNum = lineNum;

    if (mode == "read") {
        gpio.getSettings()->mode = PObject::State_t::PL_INPUT;
        
        if (gpio.initialize() != PObject::Status_t::PL_OK) {
            std::cerr << "Failed to initialize GPIO for reading\n";
            return 1;
        }

        int value = gpio.read();
        std::cout << "GPIO " << chipName << " line " << lineNum << " = " << value << std::endl;
        
        gpio.shutdown();
    } 
    else if (mode == "write-high" || mode == "write-low") {
        gpio.getSettings()->mode = PObject::State_t::PL_OUTPUT;
        
        if (gpio.initialize() != PObject::Status_t::PL_OK) {
            std::cerr << "Failed to initialize GPIO for writing\n";
            return 1;
        }

        int value = (mode == "write-high") ? 1 : 0;
        if (gpio.write(value) != PObject::Status_t::PL_OK) {
            std::cerr << "Failed to write GPIO value\n";
            gpio.shutdown();
            return 1;
        }

        std::cout << "GPIO " << chipName << " line " << lineNum << " set to " << value << std::endl;
        
        gpio.shutdown();
    }
    else {
        std::cerr << "Invalid mode: " << mode << "\n";
        return 1;
    }

    return 0;
}
