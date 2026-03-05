#ifndef LIB_PPID
#define LIB_PPID

#include "PObject.h"

typedef struct {
    float Kp;  // Proportional gain
    float Ki;  // Integral gain
    float Kd;  // Derivative gain
    float setpoint;
    float last_error;
    float integral;
    float output_min;
    float output_max;
} PIDController;

void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float output_min, float output_max);
void PID_Reset(PIDController* pid);
float PID_Compute(PIDController* pid, float measurement, float dt);

#endif // LIB_PUTILS
