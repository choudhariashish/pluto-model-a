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
        HAL_GPIO_5,      /* P9.12  GPIO60              */
        HAL_GPIO_6,      /* P9.13  GPIO31              */
        HAL_GPIO_7,      /* P9.15  GPIO48              */
        HAL_GPIO_8,      /* P9.23  GPIO49              */
        HAL_GPIO_9,      /* P9.24  GPIO15              */
        HAL_GPIO_10,     /* P9.27  GPIO115             */
        HAL_GPIO_11,     /* P9.30  GPIO112             */
        HAL_GPIO_12,     /* P8.7   GPIO66              */
        HAL_GPIO_13,     /* P8.8   GPIO67              */
        HAL_GPIO_14,     /* P8.9   GPIO69              */
        HAL_GPIO_15,     /* P8.10  GPIO68              */
        HAL_GPIO_16,     /* P8.11  GPIO45              */
        HAL_GPIO_17,     /* P8.15  GPIO47              */
        HAL_GPIO_18,     /* P8.16  GPIO46              */
        HAL_GPIO_19,     /* P8.17  GPIO27              */
        HAL_GPIO_20,     /* P8.18  GPIO65              */
        HAL_GPIO_MAX
    }

    HalGpio(const char* name) : HalObject(name) { setType(PObject::ObjectLevel::PL_HAL, PObject::ObjectHalType::PL_GPIO); }
    ~HalGpio() { settings_.risingEdgeDetection = false; };

    /// Configure the object.
    struct Settings_t
    {
        volatile PObject::State_t mode;             // PL_INPUT/PL_OUTPUT/PL_RISING_EVENT_COUNT.
        volatile uint64_t debounceTimeMs;           // Debounce time in milliseconds for rising edge events.
        volatile bool risingEdgeDetection; // Whether to detect rising edges.
    };

    /// Runtime data of the object.
    struct Data_t
    {
        volatile int valueCurrent;      // Current value of the GPIO.
        volatile int valuePrevious;     // Previous value of the GPIO.
        volatile int valueToggleCount;  // Number of times the value has toggled in debounceTimeMs.
        volatile int freqHz;            // Frequency in Hz.
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
    std::string gpioPath_ = "/sys/class/gpio/gpioX/";
    uint64_t lastEventTime_ = 0;
    std::thread monitoringThread_;
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
