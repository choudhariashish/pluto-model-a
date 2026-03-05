#include "motor_control.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <glob.h>
#include <sys/time.h>

// External global variable for signal handling
extern bool g_running;

namespace motor {

// PWM Implementation
PWM::PWM(const std::string& pin_name, const std::string& pwm_chip, int pwm_channel)
    : pin_name_(pin_name), pwm_chip_(pwm_chip), pwm_channel_(pwm_channel), initialized_(false) {
}

PWM::~PWM() {
    // Don't stop PWM on destruction - let it keep running
}

bool PWM::WriteToFile(const std::string& filename, const std::string& value) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << ": " << strerror(errno) << std::endl;
        return false;
    }
    file << value;
    file.flush();
    if (file.fail()) {
        std::cerr << "Failed to write to " << filename << std::endl;
        file.close();
        return false;
    }
    file.close();
    return true;
}

std::string PWM::GetPwmPath() const {
    // For older kernel with cape manager, use pwm_test_P9_XX path
    std::string pin_path = pin_name_;
    size_t dot_pos = pin_path.find('.');
    if (dot_pos != std::string::npos) {
        pin_path[dot_pos] = '_';
    }
    
    glob_t glob_result;
    std::string pattern = "/sys/devices/ocp.*/pwm_test_" + pin_path + ".*";
    
    if (glob(pattern.c_str(), GLOB_NOSORT, NULL, &glob_result) == 0) {
        if (glob_result.gl_pathc > 0) {
            std::string path = glob_result.gl_pathv[0];
            globfree(&glob_result);
            return path;
        }
        globfree(&glob_result);
    }
    
    return "";
}

bool PWM::Export() {
    std::string export_path = "/sys/class/pwm/" + pwm_chip_ + "/export";
    return WriteToFile(export_path, std::to_string(pwm_channel_));
}

bool PWM::Initialize() {
    if (initialized_) {
        return true;
    }
    
    // PWM overlays are already loaded, just find the path
    pwm_path_ = GetPwmPath();
    if (pwm_path_.empty()) {
        std::cerr << "Failed to find PWM path for " << pin_name_ << std::endl;
        return false;
    }
    
    // Set period to 1ms (1000000 ns) for 1kHz PWM
    if (!SetPeriod(1000000)) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool PWM::SetPeriod(long period_ns) {
    if (pwm_path_.empty()) {
        return false;
    }
    return WriteToFile(pwm_path_ + "/period", std::to_string(period_ns));
}

bool PWM::SetDutyNs(long duty_ns) {
    if (pwm_path_.empty()) {
        return false;
    }
    return WriteToFile(pwm_path_ + "/duty", std::to_string(duty_ns));
}

bool PWM::Enable() {
    if (pwm_path_.empty()) {
        return false;
    }
    return WriteToFile(pwm_path_ + "/run", "1");
}

bool PWM::Disable() {
    if (pwm_path_.empty()) {
        return false;
    }
    return WriteToFile(pwm_path_ + "/run", "0");
}

bool PWM::SetDutyCycle(int percent) {
    if (!initialized_) {
        if (!Initialize()) {
            return false;
        }
    }
    
    if (percent < 0 || percent > 100) {
        std::cerr << "Duty cycle must be 0-100" << std::endl;
        return false;
    }
    
    // Period is 1ms (1000000 ns)
    // Polarity is inverted (1), so invert the duty cycle
    // When user specifies 75%, we want ON for 75%, so write 25% to duty
    int inverted_percent = 100 - percent;
    long duty_ns = (1000000 * inverted_percent) / 100;
    
    if (!SetDutyNs(duty_ns)) {
        return false;
    }
    
    // Enable PWM if duty > 0, disable if duty = 0
    if (percent > 0) {
        return Enable();
    } else {
        return Disable();
    }
}

bool PWM::Stop() {
    if (!initialized_) {
        return true;
    }
    return Disable();
}

// GPIO Implementation
GPIO::GPIO(int gpio_num) 
    : gpio_num_(gpio_num), exported_(false), value_fd_(-1) {
    gpio_path_ = "/sys/class/gpio/gpio" + std::to_string(gpio_num_);
}

GPIO::~GPIO() {
    if (value_fd_ >= 0) {
        close(value_fd_);
    }
    // Don't unexport GPIO on destruction - let it keep its state
}

bool GPIO::WriteToFile(const std::string& filename, const std::string& value) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }
    file << value;
    file.close();
    return true;
}

std::string GPIO::ReadFromFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return "";
    }
    std::string value;
    std::getline(file, value);
    file.close();
    return value;
}

bool GPIO::Export() {
    if (exported_) {
        return true;
    }
    
    if (!WriteToFile("/sys/class/gpio/export", std::to_string(gpio_num_))) {
        return false;
    }
    
    usleep(100000);  // Wait for GPIO to be exported
    exported_ = true;
    return true;
}

bool GPIO::Unexport() {
    if (!exported_) {
        return true;
    }
    
    WriteToFile("/sys/class/gpio/unexport", std::to_string(gpio_num_));
    exported_ = false;
    return true;
}

bool GPIO::Initialize() {
    return Export();
}

bool GPIO::SetDirection(const std::string& direction) {
    if (!exported_) {
        if (!Export()) {
            return false;
        }
    }
    
    return WriteToFile(gpio_path_ + "/direction", direction);
}

bool GPIO::SetValue(int value) {
    if (!exported_) {
        if (!Export()) {
            return false;
        }
    }
    
    return WriteToFile(gpio_path_ + "/value", std::to_string(value));
}

int GPIO::GetValue() {
    if (!exported_) {
        return -1;
    }
    
    std::string value = ReadFromFile(gpio_path_ + "/value");
    if (value.empty()) {
        return -1;
    }
    
    return std::stoi(value);
}

bool GPIO::SetEdge(const std::string& edge) {
    if (!exported_) {
        if (!Export()) {
            return false;
        }
    }
    
    return WriteToFile(gpio_path_ + "/edge", edge);
}

int GPIO::GetFileDescriptor() {
    if (value_fd_ >= 0) {
        return value_fd_;
    }
    
    std::string value_path = gpio_path_ + "/value";
    value_fd_ = open(value_path.c_str(), O_RDONLY);
    return value_fd_;
}

// Tachometer Implementation
Tachometer::Tachometer(const std::string& name, int gpio_num)
    : name_(name), gpio_(gpio_num), monitoring_(false) {
}

Tachometer::~Tachometer() {
    Stop();
}

bool Tachometer::Initialize() {
    if (!gpio_.Initialize()) {
        return false;
    }
    
    if (!gpio_.SetDirection("in")) {
        return false;
    }
    
    if (!gpio_.SetEdge("rising")) {
        return false;
    }
    
    return true;
}

void Tachometer::Monitor() {
    if (!Initialize()) {
        std::cerr << "Failed to initialize tachometer for " << name_ << std::endl;
        return;
    }
    
    monitoring_ = true;
    int fd = gpio_.GetFileDescriptor();
    if (fd < 0) {
        std::cerr << "Failed to get file descriptor for " << name_ << std::endl;
        return;
    }
    
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI | POLLERR;
    
    char buf[64];
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, sizeof(buf));
    
    int edge_count = 0;
    struct timeval last_time, current_time;
    gettimeofday(&last_time, NULL);
    
    std::cout << name_ << " monitoring started (Ctrl+C to stop)..." << std::endl;
    
    while (::g_running) {
        int ret = poll(&pfd, 1, 10);  // 10ms timeout for faster response
        
        if (ret > 0) {
            if (pfd.revents & POLLPRI) {
                lseek(fd, 0, SEEK_SET);
                read(fd, buf, sizeof(buf));
                edge_count++;
            }
        }
        
        gettimeofday(&current_time, NULL);
        long elapsed_ms = (current_time.tv_sec - last_time.tv_sec) * 1000 +
                         (current_time.tv_usec - last_time.tv_usec) / 1000;
        
        if (elapsed_ms >= 200) {
            double rate = (edge_count * 1000.0) / elapsed_ms;
            std::cout << "\r" << name_ << ": " << std::fixed << std::setprecision(4) << rate << " Hz   " << std::flush;
            edge_count = 0;
            last_time = current_time;
        }
    }
    
    std::cout << std::endl;
}

void Tachometer::Stop() {
    monitoring_ = false;
}

} // namespace motor
