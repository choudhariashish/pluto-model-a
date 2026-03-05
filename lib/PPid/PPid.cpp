#include "PPid.h"

void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float output_min, float output_max)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = 0.0;
    pid->last_error = 0.0;
    pid->integral = 0.0;
    pid->output_min = output_min;
    pid->output_max = output_max;
}

void PID_Reset(PIDController* pid)
{
    pid->integral = 0.0;
    pid->last_error = 0.0;
    pid->setpoint = 0.0;
}

float PID_Compute(PIDController* pid, float measurement, float dt)
{
    float error = pid->setpoint - measurement;
    float derivative = (error - pid->last_error) / dt;

    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    // Anti-windup: Only integrate if output is not saturated
    if (output <= pid->output_max && output >= pid->output_min) {
        pid->integral += error * dt;
    }

    // Clamp the output
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }

    pid->last_error = error;
    return output;
}