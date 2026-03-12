#include "PHal.h"
#include <glob.h>

PObject::Status_t HalGpio::initialize(void)
{
#if PLATFORM == BBB
    if      (HAL_GPIO_1 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio30/"; }
    else if (HAL_GPIO_2 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio26/"; }
    else if (HAL_GPIO_3 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio14/"; }
    else if (HAL_GPIO_4 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio44/"; }
    else if (HAL_GPIO_5 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio60/"; }
    else if (HAL_GPIO_6 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio31/"; }
    else if (HAL_GPIO_7 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio48/"; }
    else if (HAL_GPIO_8 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio49/"; }
    else if (HAL_GPIO_9 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio15/"; }
    else if (HAL_GPIO_10 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio115/"; }
    else if (HAL_GPIO_11 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio112/"; }
    else if (HAL_GPIO_12 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio66/"; }
    else if (HAL_GPIO_13 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio67/"; }
    else if (HAL_GPIO_14 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio69/"; }
    else if (HAL_GPIO_15 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio68/"; }
    else if (HAL_GPIO_16 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio45/"; }
    else if (HAL_GPIO_17 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio47/"; }
    else if (HAL_GPIO_18 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio46/"; }
    else if (HAL_GPIO_19 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio27/"; }
    else if (HAL_GPIO_20 == settings_.gpio) { gpioPath_ = "/sys/class/gpio/gpio65/"; }
    else return PObject::Status_t::PL_NOT_OK;


    if (settings_.mode == PObject::State_t::PL_OUTPUT)
    {
        if (writeFile((gpioPath_ + "direction").c_str(), "out") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
    }
    else if (settings_.mode == PObject::State_t::PL_INPUT)
    {
        if (writeFile((gpioPath_ + "direction").c_str(), "in") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
    }
    else if (settings_.mode == PObject::State_t::PL_RISING_EVENT_COUNT)
    {
        if (writeFile((gpioPath_ + "direction").c_str(), "in") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
        if (writeFile((gpioPath_ + "edge").c_str(), "rising") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }

        settings_.risingEdgeDetection = true;

        // Start monitoring thread as a lambda function.
        monitoringThread_ = std::thread([this]()
        {
            // In this thread, we will monitor the GPIO pin for events
            // in debounceTimeMs period and update the freqHz count accordingly.
            while (true == settings_.risingEdgeDetection)
            {
                uint64_t startTime = PObject::getCurrentTimeMs();
                while (PObject::getCurrentTimeMs() < startTime + settings_.debounceTimeMs)
                {
                    if (data_.valueCurrent != data_.valuePrevious)
                    {
                        data_.valueToggleCount++;
                    }
                    data_.valuePrevious = data_.valueCurrent;

                    if (false == settings_.risingEdgeDetection) break;
                }
                data_.freqHz = data_.valueToggleCount * 1000 / settings_.debounceTimeMs;
                data_.valueToggleCount = 0;
            }
        });
    }
    else
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif

    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalGpio::shutdown(void)
{
    return PObject::Status_t::PL_OK;
}

int HalGpio::read(void)
{
    if (PObject::State_t::PL_INPUT == settings_.mode)
    {
#if PLATFORM == BBB
        char valueStr[16];
        if (PObject::readFile((gpioPath_ + "value").c_str(), valueStr, sizeof(valueStr)) == PObject::Status_t::PL_OK)
        {
            data_.valueCurrent = atoi(valueStr);
        }
#endif
    }
    return data_.valueCurrent;
}

PObject::Status_t HalGpio::write(int value)
{
    if (PObject::State_t::PL_OUTPUT == settings_.mode)
    {
#if PLATFORM == BBB
        // Write the value to the GPIO pin
        char valueStr[16];
        snprintf(valueStr, sizeof(valueStr), "%d", value);
        PObject::writeFile((gpioPath_ + "value").c_str(), valueStr);
#endif
    }
    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalGpio::run()
{
    return PObject::Status_t::PL_OK;
}


//---------------------------------------------------------------------------------------------------------------------
PObject::Status_t HalPwm::initialize(void)
{
#if PLATFORM == BBB
    std::string pattern;
    if      (HAL_PWM_1 == settings_.pwm) { pattern = "/sys/devices/ocp.3/pwm_test_P9_14.*/"; }
    else if (HAL_PWM_2 == settings_.pwm) { pattern = "/sys/devices/ocp.3/pwm_test_P9_16.*/"; }
    else if (HAL_PWM_3 == settings_.pwm) { pattern = "/sys/devices/ocp.3/pwm_test_P9_21.*/"; }
    else if (HAL_PWM_4 == settings_.pwm) { pattern = "/sys/devices/ocp.3/pwm_test_P9_22.*/"; }
    else if (HAL_PWM_5 == settings_.pwm) { pattern = "/sys/devices/ocp.3/pwm_test_P8_19.*/"; }
    else { return PObject::Status_t::PL_NOT_OK; }

    // Use glob to find the actual directory
    glob_t globbuf;
    if (glob(pattern.c_str(), GLOB_ONLYDIR, NULL, &globbuf) == 0 && globbuf.gl_pathc > 0)
    {
        pwmPath_ = std::string(globbuf.gl_pathv[0]) + "/";
        globfree(&globbuf);
    }
    else
    {
        if (globbuf.gl_pathc == 0) globfree(&globbuf);
        return PObject::Status_t::PL_NOT_OK;
    }

    if (writeFile((pwmPath_ + "duty").c_str(), "0") != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
    if (writeFile((pwmPath_ + "run").c_str(), "0") != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif
    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalPwm::shutdown(void)
{
#if PLATFORM == BBB
    if (writeFile((pwmPath_ + "duty").c_str(), "0") != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
    if (writeFile((pwmPath_ + "run").c_str(), "0") != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::enable(PObject::State_t state)
{
#if PLATFORM == BBB
    if (writeFile((pwmPath_ + "run").c_str(), state == PObject::State_t::PL_ENABLE ? "1" : "0") != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::setDuty(int duty)
{
#if PLATFORM == BBB
    if (writeFile((pwmPath_ + "duty").c_str(), std::to_string(duty).c_str()) != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::setDutyPercent(int dutyPercent)
{
#if PLATFORM == BBB
    // Period is fixed at 500000ns (2kHz) in init-bbb-hardware.sh
    int duty = (dutyPercent * 500000) / 100;
    if (writeFile((pwmPath_ + "duty").c_str(), std::to_string(duty).c_str()) != PObject::Status_t::PL_OK)
    {
        return PObject::Status_t::PL_NOT_OK;
    }
#endif
    return Status_t::PL_OK;
}