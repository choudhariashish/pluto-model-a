#include "PHal.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

volatile bool running = true;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received. Shutting down...\n";
    running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);

    std::cout << "=== NVIDIA Orin Nano GPIO Demo ===\n\n";

    // Example GPIO configuration for Orin Nano
    // Using gpiochip0 which is the main GPIO controller (tegra234-gpio)
    // Pin numbers are calculated as: base + port_offset + pin_offset
    // For example: GPIO3_PH.00 = 348 (base) + 43 (PORT_H offset) + 0 = 391

    // Create output GPIO (LED control example)
    HalGpio outputGpio("gpio_output");
    outputGpio.getSettings()->mode = PObject::State_t::PL_OUTPUT;
    outputGpio.getSettings()->gpioChipName = "gpiochip0";
    outputGpio.getSettings()->lineNum = 106;  // Example: GPIO09 (PAC.06)

    // Create input GPIO (button/sensor reading example)
    HalGpio inputGpio("gpio_input");
    inputGpio.getSettings()->mode = PObject::State_t::PL_INPUT;
    inputGpio.getSettings()->gpioChipName = "gpiochip0";
    inputGpio.getSettings()->lineNum = 112;  // Example: GPIO11 (PAC.04)

    // Initialize GPIOs
    std::cout << "Initializing output GPIO (line " << outputGpio.getSettings()->lineNum << ")...\n";
    if (outputGpio.initialize() != PObject::Status_t::PL_OK) {
        std::cerr << "Failed to initialize output GPIO\n";
        return 1;
    }
    std::cout << "Output GPIO initialized successfully\n";

    std::cout << "Initializing input GPIO (line " << inputGpio.getSettings()->lineNum << ")...\n";
    if (inputGpio.initialize() != PObject::Status_t::PL_OK) {
        std::cerr << "Failed to initialize input GPIO\n";
        outputGpio.shutdown();
        return 1;
    }
    std::cout << "Input GPIO initialized successfully\n\n";

    // Main demo loop
    std::cout << "Starting GPIO demo loop...\n";
    std::cout << "- Output GPIO will toggle every second\n";
    std::cout << "- Input GPIO will be read continuously\n";
    std::cout << "Press Ctrl+C to exit\n\n";

    int outputState = 0;
    int loopCount = 0;

    while (running) {
        // Toggle output GPIO
        outputGpio.write(outputState);
        std::cout << "[" << loopCount << "] Output GPIO set to: " << outputState << " | ";

        // Read input GPIO
        int inputValue = inputGpio.read();
        std::cout << "Input GPIO value: " << inputValue << std::endl;

        // Toggle state for next iteration
        outputState = !outputState;
        loopCount++;

        sleep(1);
    }

    // Cleanup
    std::cout << "\nShutting down GPIOs...\n";
    outputGpio.shutdown();
    inputGpio.shutdown();
    std::cout << "GPIO demo completed successfully\n";

    return 0;
}
