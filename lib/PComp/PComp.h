#ifndef LIB_PCOMP
#define LIB_PCOMP

#include "PObject.h"
#include "PHal.h"
#include "pid_controller.h"

class CompObject : public PObject
{
public:
    CompObject(const char* name) : PObject(name) {}
private:
};

class Drive : public CompObject
{
public:
    Drive(const char* name);

    struct Settings_t
    {
        HalPwm* m1Pwm;
        HalPwm* m2Pwm;

        HalGpio* m1DirGpio;
        HalGpio* m2DirGpio;

        HalGpio* m1TachGpio;
        HalGpio* m2TachGpio;

        PObject::State_t controlMode;    // PL_SPEED_MODE/PL_MANUAL_MODE

        // Edit directly for speed/manual mode.
        PObject::State_t m1Dir;          // PL_FORWARD/PL_REVERSE
        PObject::State_t m2Dir;          // PL_FORWARD/PL_REVERSE

        // Edit directly for manual mode.
        volatile int m1TargetDutyCycle;           // 0-100 percent
        volatile int m2TargetDutyCycle;           // 0-100 percent

        // Edit directly for speed mode.
        volatile int m1TargetCprSpeed;            // @TODO
        volatile int m2TargetCprSpeed;            // @TODO

        PIDController m1Pid;
        PIDController m2Pid;
    };
    struct Settings_t* getSettings(void) { return &settings_; }

    struct Data_t
    {
        volatile int m1CprSpeed;
        volatile int m2CprSpeed;
    };
    struct Data_t* getData(void) { return &data_; }

    Status_t initialize(void) override;
    Status_t run(void) override;
    Status_t shutdown(void) override;

private:
    struct Settings_t settings_;
    struct Data_t data_;
};

#endif // LIB_PCOMP