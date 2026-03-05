#ifndef BBB_PIN_MAP_H
#define BBB_PIN_MAP_H

#include <string>
#include <map>
#include <vector>

namespace bbb {

struct PinInfo {
    std::string pin_name;      // e.g., "P9.14"
    std::string function;      // e.g., "GPIO", "PWM", "UART", etc.
    int gpio_num;              // GPIO number (-1 if not GPIO)
    std::string pwm_chip;      // PWM chip path (empty if not PWM)
    int pwm_channel;           // PWM channel (-1 if not PWM)
    std::string notes;         // Additional information
};

class PinMap {
public:
    static const std::map<std::string, PinInfo>& GetP8Pins();
    static const std::map<std::string, PinInfo>& GetP9Pins();
    static void PrintPinMap();
    static void PrintHelp();
    static const PinInfo* GetPinInfo(const std::string& pin_name);
};

} // namespace bbb

#endif // BBB_PIN_MAP_H
