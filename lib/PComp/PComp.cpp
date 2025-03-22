#include "PComp.h"

Drive::Drive(const char* name) : CompObject(name)
{
    settings_.controlMode = PObject::State_t::PL_MANUAL_MODE;
    settings_.m1Dir = PObject::State_t::PL_FORWARD;
    settings_.m2Dir = PObject::State_t::PL_FORWARD;
    settings_.m1TargetDutyCycle = 0;
    settings_.m2TargetDutyCycle = 0;
    settings_.m1TargetCprSpeed = 0;
    settings_.m2TargetCprSpeed = 0;
}

PObject::Status_t Drive::initialize(void)
{
    settings_.m1Pwm->initialize();
    settings_.m2Pwm->initialize();

    settings_.m1DirGpio->initialize();
    settings_.m2DirGpio->initialize();

    settings_.m1TachGpio->initialize();
    settings_.m2TachGpio->initialize();

    settings_.m1Pwm->enable(PObject::State_t::PL_DISABLE);
    settings_.m2Pwm->enable(PObject::State_t::PL_DISABLE);

    settings_.m1Pwm->enable(PObject::State_t::PL_ENABLE);
    settings_.m2Pwm->enable(PObject::State_t::PL_ENABLE);

    settings_.m1Pwm->setDutyPercent(3);
    settings_.m2Pwm->setDutyPercent(3);

    PID_Init(&settings_.m1Pid, 0.4, 0.3, 0.0, 3.0, 80.0);
    PID_Init(&settings_.m2Pid, 0.4, 0.3, 0.0, 3.0, 80.0);

    return Status_t::PL_OK;
}

PObject::Status_t Drive::run(void)
{
    uint64_t t1 = PObject::getTickMs();
    int m1Count=0, m1Curr=0, m1Prev=0;
    int m2Count=0, m2Curr=0, m2Prev=0;
    while (t1 + 10 > PObject::getTickMs())
    {
        m1Curr = settings_.m1TachGpio->read();
        if (m1Curr != m1Prev) { m1Count++; m1Prev = m1Curr; }
        m2Curr = settings_.m2TachGpio->read();
        if (m2Curr != m2Prev) { m2Count++; m2Prev = m2Curr; }
    }
    data_.m1CprSpeed = m1Count;
    data_.m2CprSpeed = m2Count;

    // Perform close/open loop controls.
    if (settings_.controlMode == PL_SPEED_MODE)
    {
        settings_.m1Pid.setpoint = (float)settings_.m1TargetCprSpeed;
        settings_.m2Pid.setpoint = (float)settings_.m2TargetCprSpeed;

        float output1 = PID_Compute(&settings_.m1Pid, data_.m1CprSpeed, 0.1);
        float output2 = PID_Compute(&settings_.m2Pid, data_.m2CprSpeed, 0.1);

        settings_.m1TargetDutyCycle = (int)output1;
        settings_.m2TargetDutyCycle = (int)output2;
    }

    settings_.m1DirGpio->write((settings_.m1Dir==PObject::State_t::PL_FORWARD) ? 0 : 1);
    settings_.m2DirGpio->write((settings_.m2Dir==PObject::State_t::PL_FORWARD) ? 1 : 0);

    settings_.m1Pwm->setDutyPercent(settings_.m1TargetDutyCycle);
    settings_.m2Pwm->setDutyPercent(settings_.m2TargetDutyCycle);
    return Status_t::PL_OK;
}

PObject::Status_t Drive::shutdown(void)
{
    settings_.m1Pwm->shutdown();
    settings_.m2Pwm->shutdown();

    settings_.m1Pwm->enable(PObject::State_t::PL_DISABLE);
    settings_.m2Pwm->enable(PObject::State_t::PL_DISABLE);

    return Status_t::PL_OK;
}
