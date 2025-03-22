#include "PHal.h"

#include <gpiod.h>

PObject::Status_t HalGpio::initialize(void)
{
    settings_.chipObj = gpiod_chip_open_by_name(settings_.gpioChipName);
    if (!settings_.chipObj)
    {
        printf("Cannot open gpio file %s\n", settings_.gpioChipName);
        return PObject::Status_t::PL_NOT_OK;
    }

    settings_.lineObj = gpiod_chip_get_line(settings_.chipObj, settings_.lineNum);
    if (!settings_.lineObj)
    {
        printf("Cannot open line file %s\n", settings_.gpioChipName);
        gpiod_chip_close(settings_.chipObj);
        return PObject::Status_t::PL_NOT_OK;
    }

    if (PObject::State_t::PL_OUTPUT == settings_.mode)
    {
        int ret = gpiod_line_request_output(settings_.lineObj, "consumer", 0);    // 0 is default output.
        if (ret < 0)
        {
            printf("Request line as output failed %s\n", settings_.gpioChipName);
            gpiod_line_release(settings_.lineObj);
            gpiod_chip_close(settings_.chipObj);
            return PObject::Status_t::PL_NOT_OK;
        }
    }

    if (PObject::State_t::PL_INPUT == settings_.mode)
    {
        int ret = gpiod_line_request_input(settings_.lineObj, "consumer");
        if (ret < 0)
        {
            printf("Request line as input failed %s\n", settings_.gpioChipName);
            gpiod_line_release(settings_.lineObj);
            gpiod_chip_close(settings_.chipObj);
            return PObject::Status_t::PL_NOT_OK;
        }
    }

    if (PObject::State_t::PL_RISING_EVENT_COUNT == settings_.mode)
    {
        int ret = gpiod_line_request_rising_edge_events(settings_.lineObj, "consumer");
        if (ret < 0)
        {
            printf("Request line as rising edge events failed %s\n", settings_.gpioChipName);
            gpiod_line_release(settings_.lineObj);
            gpiod_chip_close(settings_.chipObj);
            return PObject::Status_t::PL_NOT_OK;
        }
    }
    return PObject::Status_t::PL_OK;
}

PObject::Status_t HalGpio::shutdown(void)
{
    gpiod_line_release(settings_.lineObj);
    gpiod_chip_close(settings_.chipObj);
    return PObject::Status_t::PL_OK;
}

int HalGpio::read(void)
{
    int value = 0;
    if (PObject::State_t::PL_INPUT == settings_.mode)
    {
        value = gpiod_line_get_value(settings_.lineObj);
        if (value < 0)
        {
            printf("Read line value failed %s=>%d\n", settings_.gpioChipName, settings_.lineNum);
        }
    }
    if (PObject::State_t::PL_RISING_EVENT_COUNT == settings_.mode)
    {
        struct gpiod_line_event event;
        struct timespec ts = { 3, 0 };
        int ret = gpiod_line_event_wait(settings_.lineObj, &ts);
        if (ret < 0)
        {
            printf("Wait event notification failed\n");
            value = 0;
        }
        else if (ret == 0)
        {
            value = 0;
        }
        else
        {
            ret = gpiod_line_event_read(settings_.lineObj, &event);
            if (ret == 0) data_.events++;
        }
    }
    return value;
}

PObject::Status_t HalGpio::write(int value)
{
    if (PObject::State_t::PL_OUTPUT == settings_.mode)
    {
        int ret = gpiod_line_set_value(settings_.lineObj, value);
        if (ret < 0)
        {
            printf("Set line value failed %s=>%d\n", settings_.gpioChipName, settings_.lineNum);
            return PObject::Status_t::PL_NOT_OK;
        }
    }
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