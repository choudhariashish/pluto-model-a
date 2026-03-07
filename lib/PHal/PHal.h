#ifndef LIB_PHAL
#define LIB_PHAL

#include "PObject.h"

class HalObject : public PObject
{
public:
    HalObject(const char* name) : PObject(name) {}
private:
};

class HalGpio : public HalObject
{
public:
    enum                 /*         Platform           */
    {                    /*     bbb             orin   */
        HAL_GPIO_1,      /* P9.11  GPIO30              */
        HAL_GPIO_2,      /* P8.14  GPIO26              */
        HAL_GPIO_3,      /* P9.26  GPIO14              */
        HAL_GPIO_4,      /* P8.12  GPIO44              */
        HAL_GPIO_5,      /*                            */
        HAL_GPIO_6,      /*                            */
        HAL_GPIO_7,      /*                            */
        HAL_GPIO_8,      /*                            */
        HAL_GPIO_9,      /*                            */
        HAL_GPIO_10,     /*                            */
        HAL_GPIO_MAX
    }

    HalGpio(const char* name) : HalObject(name) { setType(PObject::ObjectLevel::PL_HAL, PObject::ObjectHalType::PL_GPIO); }

    /// Configure the object.
    struct Settings_t
    {
        PObject::State_t mode;    // PL_INPUT/PL_OUTPUT/PL_RISING_EVENT_COUNT.
    };

    /// Runtime data of the object.
    struct Data_t
    {
        volatile int value;
        volatile int freqHz;
    };

    Status_t initialize(void) override;
    Status_t shutdown(void) override;

    int read(void);
    Status_t write(int value);
    Status_t run();

    Settings_t* getSettings(void) { return &settings_; }
    Data_t* getData(void) { return &data_; }

private:
    struct Settings_t settings_;
    struct Data_t data_;
};

class HalPwm : public HalObject
{
#define MAX_FILE_PATH    256
public:
    HalPwm(const char* name) : HalObject(name) { setType(PObject::ObjectLevel::PL_HAL, PObject::ObjectHalType::PL_PWM); }
    Status_t initialize(void) override;
    Status_t shutdown(void) override;

    // PObject::State_t::ENABLE/DISABLE.
    Status_t enable(PObject::State_t state);
    Status_t setPeriod(int period);
    Status_t setDuty(int duty);
    Status_t setDutyPercent(int dutyPercent);

    struct Settings_t
    {
        const char* modulePath;
        char periodFilePath[MAX_FILE_PATH];
        char dutyFilePath[MAX_FILE_PATH];
        char enableFilePath[MAX_FILE_PATH];
        FILE* periodFile;
        FILE* dutyFile;
        FILE* enableFile;
        int period;
        int duty;
        PObject::State_t enable; // PObject::State_t::ENABLE/DISABLE.
    };
    Settings_t* getSettings(void) { return &settings_; }

private:
    struct Settings_t settings_;
};
#endif // LIB_PHAL
