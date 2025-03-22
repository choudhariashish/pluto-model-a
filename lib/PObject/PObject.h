#ifndef LIB_POBJECT
#define LIB_POBJECT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>

class PObject
{
public:
    // For API return status.
    enum Status_t { PL_OK, PL_NOT_OK };
    // For stateful processes.
    enum State_t { PL_BUSY, PL_READY, PL_DONE, PL_ENABLE, PL_DISABLE, PL_ERROR, PL_INPUT, PL_OUTPUT, PL_RISING_EVENT_COUNT, PL_FORWARD, PL_REVERSE, PL_STOP, PL_PAUSE, PL_MANUAL_MODE, PL_SPEED_MODE };

    PObject(const char* name) { name_ = name; }

    // Object is categorized by a level and a type.
    enum ObjectLevel { PL_HAL, PL_COMP, PL_SUBSYS };
    enum ObjectHalType { PL_PWM, PL_GPIO };
    enum ObjectCompType { PL_DC_MOTOR };
    enum ObjectSubsysType { PL_SM };

    struct ObjectType { int olevel; int otype; };
    void setType(int l, int t) { objectType_.olevel = l; objectType_.otype = t; }
    struct ObjectType getType(void) { return objectType_; }

    virtual Status_t initialize(void) { return Status_t::PL_NOT_OK; }
    virtual Status_t run(void) { return Status_t::PL_NOT_OK; }
    virtual Status_t shutdown(void) { return Status_t::PL_NOT_OK; }

    static void setupTimeoutControl(uint32_t millisec);
    static void timeout(void);
    static uint64_t getTickMs(void);
    static uint64_t getTickPeriodMs(void);
    volatile static uint64_t tickMs;
    volatile static uint64_t tickPeriodMs;

private:
    struct ObjectType objectType_;
    const char* name_;
};

#endif // LIB_POBJECT