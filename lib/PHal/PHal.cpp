#include "PHal.h"

PObject::Status_t HalGpio::initialize(void)
{
#if PLATFORM == BBB
    std::string gpioPath = "/sys/class/gpio/gpioX/";

    if      (HAL_GPIO_1 == settings_.gpio) { gpioPath = "/sys/class/gpio/gpio30/"; }
    else if (HAL_GPIO_2 == settings_.gpio) { gpioPath = "/sys/class/gpio/gpio26/"; }
    else if (HAL_GPIO_3 == settings_.gpio) { gpioPath = "/sys/class/gpio/gpio14/"; }
    else if (HAL_GPIO_4 == settings_.gpio) { gpioPath = "/sys/class/gpio/gpio44/"; }
    else return PObject::Status_t::PL_NOT_OK;


    if (settings_.mode == PObject::State_t::PL_OUTPUT)
    {
        if (writeFile((gpioPath + "direction").c_str(), "out") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
    }
    else if (settings_.mode == PObject::State_t::PL_INPUT)
    {
        if (writeFile((gpioPath + "direction").c_str(), "in") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
    }
    else if (settings_.mode == PObject::State_t::PL_RISING_EVENT_COUNT)
    {
        if (writeFile((gpioPath + "direction").c_str(), "in") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
        if (writeFile((gpioPath + "edge").c_str(), "rising") != PObject::Status_t::PL_OK)
        {
            return PObject::Status_t::PL_NOT_OK;
        }
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
    }
    if (PObject::State_t::PL_RISING_EVENT_COUNT == settings_.mode)
    {
    }
    return value;
}

PObject::Status_t HalGpio::write(int value)
{
    if (PObject::State_t::PL_OUTPUT == settings_.mode)
    {
    }
    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalGpio::run()
{
    return PObject::Status_t::PL_OK;
}





















PObject::Status_t HalPwm::initialize(void)
{
    snprintf(settings_.periodFilePath, MAX_FILE_PATH, "%s/period", settings_.modulePath);
    snprintf(settings_.dutyFilePath, MAX_FILE_PATH, "%s/duty_cycle", settings_.modulePath);
    snprintf(settings_.enableFilePath, MAX_FILE_PATH, "%s/enable", settings_.modulePath);

    settings_.periodFile = fopen(settings_.periodFilePath, "w");
    if (settings_.periodFile == NULL) { printf("Cannot open file %s\n", settings_.periodFilePath); exit(1); }

    settings_.dutyFile = fopen(settings_.dutyFilePath, "w");
    if (settings_.dutyFile == NULL) { printf("Cannot open file %s\n", settings_.dutyFilePath); exit(1); }

    settings_.enableFile = fopen(settings_.enableFilePath, "w");
    if (settings_.enableFile == NULL) { printf("Cannot open file %s\n", settings_.enableFilePath); exit(1); }

    setPeriod(settings_.period);
    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalPwm::shutdown(void)
{
    enable(PObject::State_t::PL_DISABLE);
    setPeriod(10000);
    setDuty(0);

    if (NULL != settings_.periodFile) fclose(settings_.periodFile);
    if (NULL != settings_.dutyFile) fclose(settings_.dutyFile);
    if (NULL != settings_.enableFile) fclose(settings_.enableFile);
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::enable(PObject::State_t state)
{
    if (NULL == settings_.enableFile) return Status_t::PL_NOT_OK;
    fprintf(settings_.enableFile, "%d", (PObject::State_t::PL_ENABLE==state) ? 1 : 0);
    settings_.enable = state;
    fflush(settings_.enableFile);
    setDuty(0);
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::setPeriod(int period)
{
    if (NULL == settings_.periodFile) return Status_t::PL_NOT_OK;
    fprintf(settings_.periodFile, "%d", period);
    settings_.period = period;
    fflush(settings_.periodFile);
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::setDuty(int duty)
{
    if (NULL == settings_.enableFile) return Status_t::PL_NOT_OK;
    fprintf(settings_.dutyFile, "%d", duty);
    settings_.duty = duty;
    fflush(settings_.dutyFile);
    return Status_t::PL_OK;
}

PObject::Status_t HalPwm::setDutyPercent(int dutyPercent)
{
    setDuty(dutyPercent * settings_.period / 100.0);
    return Status_t::PL_OK;
}