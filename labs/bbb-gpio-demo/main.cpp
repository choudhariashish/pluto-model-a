#include "bbb_pin_map.h"
#include "PHal.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace bbb;

int cmd_help(const std::vector<std::string>& args) {
    PinMap::PrintHelp();
    std::cout << "\nGPIO COMMANDS:" << std::endl;
    std::cout << "  read <gpio_num> <value>  - Read GPIO value (1-20 for HAL_GPIO_1 to HAL_GPIO_20)" << std::endl;
    std::cout << "  write <gpio_num> <value> - Write GPIO value (1-20 for HAL_GPIO_1 to HAL_GPIO_20, value: 0 or 1)" << std::endl;
    std::cout << "\nEXAMPLES:" << std::endl;
    std::cout << "  bbb-gpio-demo read 1      # Read HAL_GPIO_1 (P9.11, GPIO30)" << std::endl;
    std::cout << "  bbb-gpio-demo write 5 1   # Write 1 to HAL_GPIO_5 (P9.12, GPIO60)" << std::endl;
    return 0;
}

int cmd_pinmap(const std::vector<std::string>& args) {
    PinMap::PrintPinMap();
    return 0;
}

int cmd_read(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "Error: read command requires GPIO number" << std::endl;
        std::cerr << "Usage: bbb-gpio-demo read <gpio_num>" << std::endl;
        std::cerr << "Example: bbb-gpio-demo read 1" << std::endl;
        return 1;
    }

    int gpio_num = std::atoi(args[2].c_str());
    if (gpio_num < 1 || gpio_num > 20) {
        std::cerr << "Error: GPIO number must be between 1 and 20" << std::endl;
        return 1;
    }

    HalGpio gpio("gpio_read");
    HalGpio::Settings_t* settings = gpio.getSettings();
    settings->gpio = gpio_num - 1;
    settings->mode = PObject::State_t::PL_INPUT;

    if (gpio.initialize() != PObject::Status_t::PL_OK) {
        std::cerr << "Error: Failed to initialize GPIO " << gpio_num << std::endl;
        return 1;
    }

    int value = gpio.read();
    std::cout << "GPIO " << gpio_num << " value: " << value << std::endl;

    gpio.shutdown();
    return 0;
}

int cmd_write(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        std::cerr << "Error: write command requires GPIO number and value" << std::endl;
        std::cerr << "Usage: bbb-gpio-demo write <gpio_num> <value>" << std::endl;
        std::cerr << "Example: bbb-gpio-demo write 5 1" << std::endl;
        return 1;
    }

    int gpio_num = std::atoi(args[2].c_str());
    int value = std::atoi(args[3].c_str());

    if (gpio_num < 1 || gpio_num > 20) {
        std::cerr << "Error: GPIO number must be between 1 and 20" << std::endl;
        return 1;
    }

    if (value != 0 && value != 1) {
        std::cerr << "Error: Value must be 0 or 1" << std::endl;
        return 1;
    }

    HalGpio gpio("gpio_write");
    HalGpio::Settings_t* settings = gpio.getSettings();
    settings->gpio = gpio_num - 1;
    settings->mode = PObject::State_t::PL_OUTPUT;

    if (gpio.initialize() != PObject::Status_t::PL_OK) {
        std::cerr << "Error: Failed to initialize GPIO " << gpio_num << std::endl;
        return 1;
    }

    if (gpio.write(value) != PObject::Status_t::PL_OK) {
        std::cerr << "Error: Failed to write to GPIO " << gpio_num << std::endl;
        gpio.shutdown();
        return 1;
    }

    std::cout << "GPIO " << gpio_num << " set to: " << value << std::endl;

    gpio.shutdown();
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cmd_help({});
        return 1;
    }

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    std::string command = args[1];

    if (command == "help") {
        return cmd_help(args);
    } else if (command == "pinmap") {
        return cmd_pinmap(args);
    } else if (command == "read") {
        return cmd_read(args);
    } else if (command == "write") {
        return cmd_write(args);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        cmd_help(args);
        return 1;
    }

    return 0;
}
