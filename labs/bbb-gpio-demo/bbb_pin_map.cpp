#include "bbb_pin_map.h"
#include <iostream>
#include <iomanip>

namespace bbb {

static const std::map<std::string, PinInfo> p8_pins = {
    {"P8.1",  {"P8.1",  "GND", -1, "", -1, "Ground"}},
    {"P8.2",  {"P8.2",  "GND", -1, "", -1, "Ground"}},
    {"P8.3",  {"P8.3",  "GPIO", 38, "", -1, "GPIO1_6"}},
    {"P8.4",  {"P8.4",  "GPIO", 39, "", -1, "GPIO1_7"}},
    {"P8.5",  {"P8.5",  "GPIO", 34, "", -1, "GPIO1_2"}},
    {"P8.6",  {"P8.6",  "GPIO", 35, "", -1, "GPIO1_3"}},
    {"P8.7",  {"P8.7",  "GPIO", 66, "", -1, "GPIO2_2"}},
    {"P8.8",  {"P8.8",  "GPIO", 67, "", -1, "GPIO2_3"}},
    {"P8.9",  {"P8.9",  "GPIO", 69, "", -1, "GPIO2_5"}},
    {"P8.10", {"P8.10", "GPIO", 68, "", -1, "GPIO2_4"}},
    {"P8.11", {"P8.11", "GPIO", 45, "", -1, "GPIO1_13"}},
    {"P8.12", {"P8.12", "GPIO", 44, "", -1, "GPIO1_12"}},
    {"P8.13", {"P8.13", "PWM/GPIO", 23, "pwmchip2", 1, "ehrpwm2_b"}},
    {"P8.14", {"P8.14", "GPIO", 26, "", -1, "GPIO0_26"}},
    {"P8.15", {"P8.15", "GPIO", 47, "", -1, "GPIO1_15"}},
    {"P8.16", {"P8.16", "GPIO", 46, "", -1, "GPIO1_14"}},
    {"P8.17", {"P8.17", "GPIO", 27, "", -1, "GPIO0_27"}},
    {"P8.18", {"P8.18", "GPIO", 65, "", -1, "GPIO2_1"}},
    {"P8.19", {"P8.19", "PWM/GPIO", 22, "pwmchip2", 0, "ehrpwm2_a"}},
    {"P8.20", {"P8.20", "GPIO", 63, "", -1, "GPIO1_31"}},
    {"P8.21", {"P8.21", "GPIO", 62, "", -1, "GPIO1_30"}},
    {"P8.22", {"P8.22", "GPIO", 37, "", -1, "GPIO1_5"}},
    {"P8.23", {"P8.23", "GPIO", 36, "", -1, "GPIO1_4"}},
    {"P8.24", {"P8.24", "GPIO", 33, "", -1, "GPIO1_1"}},
    {"P8.25", {"P8.25", "GPIO", 32, "", -1, "GPIO1_0"}},
    {"P8.26", {"P8.26", "GPIO", 61, "", -1, "GPIO1_29"}},
    {"P8.27", {"P8.27", "GPIO", 86, "", -1, "GPIO2_22"}},
    {"P8.28", {"P8.28", "GPIO", 88, "", -1, "GPIO2_24"}},
    {"P8.29", {"P8.29", "GPIO", 87, "", -1, "GPIO2_23"}},
    {"P8.30", {"P8.30", "GPIO", 89, "", -1, "GPIO2_25"}},
    {"P8.31", {"P8.31", "GPIO", 10, "", -1, "GPIO0_10"}},
    {"P8.32", {"P8.32", "GPIO", 11, "", -1, "GPIO0_11"}},
    {"P8.33", {"P8.33", "GPIO", 9, "", -1, "GPIO0_9"}},
    {"P8.34", {"P8.34", "GPIO", 81, "", -1, "GPIO2_17"}},
    {"P8.35", {"P8.35", "GPIO", 8, "", -1, "GPIO0_8"}},
    {"P8.36", {"P8.36", "GPIO", 80, "", -1, "GPIO2_16"}},
    {"P8.37", {"P8.37", "GPIO", 78, "", -1, "GPIO2_14"}},
    {"P8.38", {"P8.38", "GPIO", 79, "", -1, "GPIO2_15"}},
    {"P8.39", {"P8.39", "GPIO", 76, "", -1, "GPIO2_12"}},
    {"P8.40", {"P8.40", "GPIO", 77, "", -1, "GPIO2_13"}},
    {"P8.41", {"P8.41", "GPIO", 74, "", -1, "GPIO2_10"}},
    {"P8.42", {"P8.42", "GPIO", 75, "", -1, "GPIO2_11"}},
    {"P8.43", {"P8.43", "GPIO", 72, "", -1, "GPIO2_8"}},
    {"P8.44", {"P8.44", "GPIO", 73, "", -1, "GPIO2_9"}},
    {"P8.45", {"P8.45", "GPIO", 70, "", -1, "GPIO2_6"}},
    {"P8.46", {"P8.46", "GPIO", 71, "", -1, "GPIO2_7"}},
};

static const std::map<std::string, PinInfo> p9_pins = {
    {"P9.1",  {"P9.1",  "GND", -1, "", -1, "Ground"}},
    {"P9.2",  {"P9.2",  "GND", -1, "", -1, "Ground"}},
    {"P9.3",  {"P9.3",  "VDD_3V3", -1, "", -1, "3.3V Power"}},
    {"P9.4",  {"P9.4",  "VDD_3V3", -1, "", -1, "3.3V Power"}},
    {"P9.5",  {"P9.5",  "VDD_5V", -1, "", -1, "5V Power"}},
    {"P9.6",  {"P9.6",  "VDD_5V", -1, "", -1, "5V Power"}},
    {"P9.7",  {"P9.7",  "SYS_5V", -1, "", -1, "5V System"}},
    {"P9.8",  {"P9.8",  "SYS_5V", -1, "", -1, "5V System"}},
    {"P9.9",  {"P9.9",  "PWR_BUT", -1, "", -1, "Power Button"}},
    {"P9.10", {"P9.10", "SYS_RESET", -1, "", -1, "System Reset"}},
    {"P9.11", {"P9.11", "GPIO", 30, "", -1, "GPIO0_30"}},
    {"P9.12", {"P9.12", "GPIO", 60, "", -1, "GPIO1_28"}},
    {"P9.13", {"P9.13", "GPIO", 31, "", -1, "GPIO0_31"}},
    {"P9.14", {"P9.14", "PWM/GPIO", 50, "pwmchip1", 0, "ehrpwm1_a"}},
    {"P9.15", {"P9.15", "GPIO", 48, "", -1, "GPIO1_16"}},
    {"P9.16", {"P9.16", "PWM/GPIO", 51, "pwmchip1", 1, "ehrpwm1_b"}},
    {"P9.17", {"P9.17", "GPIO", 5, "", -1, "GPIO0_5"}},
    {"P9.18", {"P9.18", "GPIO", 4, "", -1, "GPIO0_4"}},
    {"P9.19", {"P9.19", "I2C", -1, "", -1, "I2C2_SCL"}},
    {"P9.20", {"P9.20", "I2C", -1, "", -1, "I2C2_SDA"}},
    {"P9.21", {"P9.21", "PWM/GPIO", 3, "pwmchip0", 1, "ehrpwm0_b"}},
    {"P9.22", {"P9.22", "PWM/GPIO", 2, "pwmchip0", 0, "ehrpwm0_a"}},
    {"P9.23", {"P9.23", "GPIO", 49, "", -1, "GPIO1_17"}},
    {"P9.24", {"P9.24", "GPIO", 15, "", -1, "GPIO0_15"}},
    {"P9.25", {"P9.25", "GPIO", 117, "", -1, "GPIO3_21"}},
    {"P9.26", {"P9.26", "GPIO", 14, "", -1, "GPIO0_14"}},
    {"P9.27", {"P9.27", "GPIO", 115, "", -1, "GPIO3_19"}},
    {"P9.28", {"P9.28", "GPIO", 113, "", -1, "GPIO3_17"}},
    {"P9.29", {"P9.29", "GPIO", 111, "", -1, "GPIO3_15"}},
    {"P9.30", {"P9.30", "GPIO", 112, "", -1, "GPIO3_16"}},
    {"P9.31", {"P9.31", "GPIO", 110, "", -1, "GPIO3_14"}},
    {"P9.41", {"P9.41", "GPIO", 20, "", -1, "GPIO0_20"}},
    {"P9.42", {"P9.42", "GPIO", 7, "", -1, "GPIO0_7"}},
};

const std::map<std::string, PinInfo>& PinMap::GetP8Pins() {
    return p8_pins;
}

const std::map<std::string, PinInfo>& PinMap::GetP9Pins() {
    return p9_pins;
}

const PinInfo* PinMap::GetPinInfo(const std::string& pin_name) {
    auto it = p8_pins.find(pin_name);
    if (it != p8_pins.end()) {
        return &it->second;
    }
    it = p9_pins.find(pin_name);
    if (it != p9_pins.end()) {
        return &it->second;
    }
    return nullptr;
}

void PinMap::PrintPinMap() {
    std::cout << "\n=== BeagleBone Black Pin Map ===\n\n";
    
    // P8 Header - side by side layout
    std::cout << "P8 Header:\n";
    std::cout << "Row1  Pin      Function    GPIO#  PWM            Notes              ||  Row2  Pin      Function    GPIO#  PWM            Notes\n";
    std::cout << std::string(150, '=') << "\n";
    
    for (int i = 1; i <= 46; i += 2) {
        std::string pin_odd = "P8." + std::to_string(i);
        std::string pin_even = "P8." + std::to_string(i + 1);
        
        std::map<std::string, PinInfo>::const_iterator it_odd = p8_pins.find(pin_odd);
        std::map<std::string, PinInfo>::const_iterator it_even = p8_pins.find(pin_even);
        
        // Row 1 (odd pins)
        if (it_odd != p8_pins.end()) {
            const PinInfo& info = it_odd->second;
            std::cout << std::left << std::setw(6) << i
                      << std::setw(9) << pin_odd
                      << std::setw(12) << info.function
                      << std::setw(7) << (info.gpio_num >= 0 ? std::to_string(info.gpio_num) : "-")
                      << std::setw(15) << (info.pwm_chip.empty() ? "-" : info.pwm_chip + "/" + std::to_string(info.pwm_channel))
                      << std::setw(20) << info.notes;
        } else {
            std::cout << std::setw(69) << " ";
        }
        
        std::cout << "||  ";
        
        // Row 2 (even pins)
        if (it_even != p8_pins.end()) {
            const PinInfo& info = it_even->second;
            std::cout << std::left << std::setw(6) << (i + 1)
                      << std::setw(9) << pin_even
                      << std::setw(12) << info.function
                      << std::setw(7) << (info.gpio_num >= 0 ? std::to_string(info.gpio_num) : "-")
                      << std::setw(15) << (info.pwm_chip.empty() ? "-" : info.pwm_chip + "/" + std::to_string(info.pwm_channel))
                      << info.notes;
        }
        
        std::cout << "\n";
    }
    
    std::cout << "\n";
    
    // P9 Header - side by side layout
    std::cout << "P9 Header:\n";
    std::cout << "Row1  Pin      Function    GPIO#  PWM            Notes              ||  Row2  Pin      Function    GPIO#  PWM            Notes\n";
    std::cout << std::string(150, '=') << "\n";
    
    for (int i = 1; i <= 42; i += 2) {
        std::string pin_odd = "P9." + std::to_string(i);
        std::string pin_even = "P9." + std::to_string(i + 1);
        
        std::map<std::string, PinInfo>::const_iterator it_odd = p9_pins.find(pin_odd);
        std::map<std::string, PinInfo>::const_iterator it_even = p9_pins.find(pin_even);
        
        // Row 1 (odd pins)
        if (it_odd != p9_pins.end()) {
            const PinInfo& info = it_odd->second;
            std::cout << std::left << std::setw(6) << i
                      << std::setw(9) << pin_odd
                      << std::setw(12) << info.function
                      << std::setw(7) << (info.gpio_num >= 0 ? std::to_string(info.gpio_num) : "-")
                      << std::setw(15) << (info.pwm_chip.empty() ? "-" : info.pwm_chip + "/" + std::to_string(info.pwm_channel))
                      << std::setw(20) << info.notes;
        } else {
            std::cout << std::setw(69) << " ";
        }
        
        std::cout << "||  ";
        
        // Row 2 (even pins)
        if (it_even != p9_pins.end()) {
            const PinInfo& info = it_even->second;
            std::cout << std::left << std::setw(6) << (i + 1)
                      << std::setw(9) << pin_even
                      << std::setw(12) << info.function
                      << std::setw(7) << (info.gpio_num >= 0 ? std::to_string(info.gpio_num) : "-")
                      << std::setw(15) << (info.pwm_chip.empty() ? "-" : info.pwm_chip + "/" + std::to_string(info.pwm_channel))
                      << info.notes;
        }
        
        std::cout << "\n";
    }
    
    std::cout << "\n";
}

void PinMap::PrintHelp() {
    std::cout << "\nBeagleBone Black Pin Map Tool" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "\nUSAGE:" << std::endl;
    std::cout << "  bbb-gpio-demo <command>" << std::endl;
    std::cout << "\nCOMMANDS:" << std::endl;
    std::cout << "  help       - Show this help message" << std::endl;
    std::cout << "  pinmap     - Display P8/P9 header pin mapping" << std::endl;
    std::cout << "\nDESCRIPTION:" << std::endl;
    std::cout << "  This tool displays the BeagleBone Black P8 and P9 header pin mappings," << std::endl;
    std::cout << "  showing pin numbers, GPIO assignments, PWM capabilities, and functions." << std::endl;
    std::cout << "\nEXAMPLE:" << std::endl;
    std::cout << "  bbb-gpio-demo pinmap" << std::endl;
    std::cout << std::endl;
}

} // namespace bbb
