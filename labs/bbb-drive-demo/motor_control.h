#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <string>

namespace motor {

// PWM control for motor speed
class PWM {
public:
    PWM(const std::string& pin_name, const std::string& pwm_chip, int pwm_channel);
    ~PWM();
    
    bool Initialize();
    bool SetDutyCycle(int percent);  // 0-100, 0 stops PWM
    bool Stop();
    
private:
    std::string pin_name_;
    std::string pwm_chip_;
    int pwm_channel_;
    std::string pwm_path_;
    bool initialized_;
    
    bool Export();
    bool SetPeriod(long period_ns);
    bool SetDutyNs(long duty_ns);
    bool Enable();
    bool Disable();
    std::string GetPwmPath() const;
    bool WriteToFile(const std::string& filename, const std::string& value);
};

// GPIO control for motor direction
class GPIO {
public:
    GPIO(int gpio_num);
    ~GPIO();
    
    bool Initialize();
    bool SetDirection(const std::string& direction);  // "in" or "out"
    bool SetValue(int value);  // 0 or 1
    int GetValue();
    bool SetEdge(const std::string& edge);  // "none", "rising", "falling", "both"
    int GetFileDescriptor();
    
private:
    int gpio_num_;
    std::string gpio_path_;
    bool exported_;
    int value_fd_;
    
    bool Export();
    bool Unexport();
    bool WriteToFile(const std::string& filename, const std::string& value);
    std::string ReadFromFile(const std::string& filename);
};

// Tachometer monitoring
class Tachometer {
public:
    Tachometer(const std::string& name, int gpio_num);
    ~Tachometer();
    
    bool Initialize();
    void Monitor();  // Blocking call that monitors and prints rate
    void Stop();
    
private:
    std::string name_;
    GPIO gpio_;
    bool monitoring_;
};

} // namespace motor

#endif // MOTOR_CONTROL_H
