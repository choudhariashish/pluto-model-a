#include "PHal.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pwm_num> <duty_percent>" << std::endl;
        std::cerr << "  pwm_num: 1-5 (HAL_PWM_1 to HAL_PWM_5)" << std::endl;
        std::cerr << "    1 = P9.14 (PWM1A)" << std::endl;
        std::cerr << "    2 = P9.16 (PWM1B)" << std::endl;
        std::cerr << "    3 = P9.21 (PWM2A)" << std::endl;
        std::cerr << "    4 = P9.22 (PWM2B)" << std::endl;
        std::cerr << "    5 = P8.19 (PWM3A)" << std::endl;
        std::cerr << "  duty_percent: 0-100 (0 disables PWM)" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Examples:" << std::endl;
        std::cerr << "  " << argv[0] << " 1 50   # Set PWM1 (P9.14) to 50% duty" << std::endl;
        std::cerr << "  " << argv[0] << " 3 0    # Disable PWM3 (P9.21)" << std::endl;
        return 1;
    }

    int pwm_num = std::atoi(argv[1]);
    int duty_percent = std::atoi(argv[2]);

    if (pwm_num < 1 || pwm_num > 5) {
        std::cerr << "Error: PWM number must be between 1 and 5" << std::endl;
        return 1;
    }

    if (duty_percent < 0 || duty_percent > 100) {
        std::cerr << "Error: Duty percent must be between 0 and 100" << std::endl;
        return 1;
    }

    HalPwm pwm("pwm_control");
    HalPwm::Settings_t* settings = pwm.getSettings();
    settings->pwm = pwm_num - 1;

    if (pwm.initialize() != PObject::Status_t::PL_OK) {
        std::cerr << "Error: Failed to initialize PWM " << pwm_num << std::endl;
        std::cerr << "Make sure init-bbb-hardware.sh has been run to load PWM overlays" << std::endl;
        return 1;
    }

    if (duty_percent == 0) {
        if (pwm.enable(PObject::State_t::PL_DISABLE) != PObject::Status_t::PL_OK) {
            std::cerr << "Error: Failed to disable PWM " << pwm_num << std::endl;
            pwm.shutdown();
            return 1;
        }
        std::cout << "PWM " << pwm_num << " disabled" << std::endl;
    } else {
        if (pwm.setDutyPercent(duty_percent) != PObject::Status_t::PL_OK) {
            std::cerr << "Error: Failed to set duty cycle for PWM " << pwm_num << std::endl;
            pwm.shutdown();
            return 1;
        }
        if (pwm.enable(PObject::State_t::PL_ENABLE) != PObject::Status_t::PL_OK) {
            std::cerr << "Error: Failed to enable PWM " << pwm_num << std::endl;
            pwm.shutdown();
            return 1;
        }
        std::cout << "PWM " << pwm_num << " set to " << duty_percent << "% duty cycle" << std::endl;
        std::cout << "Keeping PWM active for 5 seconds..." << std::endl;
        sleep(5);
        std::cout << "Done." << std::endl;
    }

    pwm.shutdown();
    return 0;
}
