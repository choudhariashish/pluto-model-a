#include "motor_control.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <signal.h>
#include <thread>
#include <poll.h>
#include <sys/time.h>
#include <unistd.h>

using namespace motor;

bool g_running = true;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\nReceived Ctrl+C, stopping..." << std::endl;
        g_running = false;
    }
}

void print_help() {
    std::cout << "\nBeagleBone Black Drive Demo" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "\nUSAGE:" << std::endl;
    std::cout << "  bbb-drive-demo <command> [options]" << std::endl;
    std::cout << "\nCOMMANDS:" << std::endl;
    std::cout << "  help                    - Show this help message" << std::endl;
    std::cout << "\n  Motor 1 (M1) - PWM: P9.14, Direction: P9.11, Tach: P8.14" << std::endl;
    std::cout << "  m1 <0-100>              - Set M1 PWM duty cycle (0=stop)" << std::endl;
    std::cout << "  m1 forward              - Set M1 direction forward (GPIO high)" << std::endl;
    std::cout << "  m1 reverse              - Set M1 direction reverse (GPIO low)" << std::endl;
    std::cout << "  m1 monitor              - Monitor M1 tachometer (Ctrl+C to stop)" << std::endl;
    std::cout << "\n  Motor 2 (M2) - PWM: P9.21, Direction: P9.26, Tach: P8.12" << std::endl;
    std::cout << "  m2 <0-100>              - Set M2 PWM duty cycle (0=stop)" << std::endl;
    std::cout << "  m2 forward              - Set M2 direction forward (GPIO high)" << std::endl;
    std::cout << "  m2 reverse              - Set M2 direction reverse (GPIO low)" << std::endl;
    std::cout << "  m2 monitor              - Monitor M2 tachometer (Ctrl+C to stop)" << std::endl;
    std::cout << "\n  Both Motors" << std::endl;
    std::cout << "  m monitor               - Monitor both M1 and M2 tachometers" << std::endl;
    std::cout << "\nEXAMPLES:" << std::endl;
    std::cout << "  bbb-drive-demo m1 50              # Set M1 to 50% duty" << std::endl;
    std::cout << "  bbb-drive-demo m1 forward         # Set M1 forward" << std::endl;
    std::cout << "  bbb-drive-demo m2 75              # Set M2 to 75% duty" << std::endl;
    std::cout << "  bbb-drive-demo m1 monitor         # Monitor M1 tach" << std::endl;
    std::cout << "  bbb-drive-demo m monitor          # Monitor both tachs" << std::endl;
    std::cout << std::endl;
}

int cmd_m1_duty(int duty) {
    PWM pwm("P9.14", "pwmchip1", 0);
    if (!pwm.SetDutyCycle(duty)) {
        std::cerr << "Failed to set M1 duty cycle" << std::endl;
        return 1;
    }
    std::cout << "M1 duty cycle set to " << duty << "%" << std::endl;
    return 0;
}

int cmd_m2_duty(int duty) {
    PWM pwm("P9.21", "pwmchip0", 1);
    if (!pwm.SetDutyCycle(duty)) {
        std::cerr << "Failed to set M2 duty cycle" << std::endl;
        return 1;
    }
    std::cout << "M2 duty cycle set to " << duty << "%" << std::endl;
    return 0;
}

int cmd_m1_direction(bool forward) {
    GPIO gpio(30);  // P9.11 = GPIO0_30
    if (!gpio.Initialize()) {
        std::cerr << "Failed to initialize M1 direction GPIO" << std::endl;
        return 1;
    }
    if (!gpio.SetDirection("out")) {
        std::cerr << "Failed to set M1 direction GPIO as output" << std::endl;
        return 1;
    }
    if (!gpio.SetValue(forward ? 1 : 0)) {
        std::cerr << "Failed to set M1 direction" << std::endl;
        return 1;
    }
    std::cout << "M1 direction set to " << (forward ? "forward" : "reverse") << std::endl;
    return 0;
}

int cmd_m2_direction(bool forward) {
    GPIO gpio(14);  // P9.26 = GPIO0_14
    if (!gpio.Initialize()) {
        std::cerr << "Failed to initialize M2 direction GPIO" << std::endl;
        return 1;
    }
    if (!gpio.SetDirection("out")) {
        std::cerr << "Failed to set M2 direction GPIO as output" << std::endl;
        return 1;
    }
    // M2 has inverted polarity: forward=0, reverse=1
    if (!gpio.SetValue(forward ? 0 : 1)) {
        std::cerr << "Failed to set M2 direction" << std::endl;
        return 1;
    }
    std::cout << "M2 direction set to " << (forward ? "forward" : "reverse") << std::endl;
    return 0;
}

int cmd_m1_monitor() {
    signal(SIGINT, signal_handler);
    Tachometer tach("M1", 26);  // P8.14 = GPIO0_26
    tach.Monitor();
    return 0;
}

int cmd_m2_monitor() {
    signal(SIGINT, signal_handler);
    Tachometer tach("M2", 44);  // P8.12 = GPIO1_12
    tach.Monitor();
    return 0;
}

void monitor_tach(const std::string& name, int gpio_num) {
    Tachometer tach(name, gpio_num);
    if (!tach.Initialize()) {
        std::cerr << "Failed to initialize " << name << " tachometer" << std::endl;
        return;
    }
    
    int fd = GPIO(gpio_num).GetFileDescriptor();
    if (fd < 0) {
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
    
    while (g_running) {
        int ret = poll(&pfd, 1, 100);
        
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
        
        if (elapsed_ms >= 1000) {
            double rate = (edge_count * 1000.0) / elapsed_ms;
            // This will be printed by the main thread
            edge_count = 0;
            last_time = current_time;
        }
    }
}

int cmd_both_monitor() {
    signal(SIGINT, signal_handler);
    
    GPIO gpio_m1(26);  // P8.14
    GPIO gpio_m2(44);  // P8.12
    
    if (!gpio_m1.Initialize() || !gpio_m2.Initialize()) {
        std::cerr << "Failed to initialize tachometer GPIOs" << std::endl;
        return 1;
    }
    
    if (!gpio_m1.SetDirection("in") || !gpio_m2.SetDirection("in")) {
        std::cerr << "Failed to set GPIO direction" << std::endl;
        return 1;
    }
    
    if (!gpio_m1.SetEdge("rising") || !gpio_m2.SetEdge("rising")) {
        std::cerr << "Failed to set GPIO edge detection" << std::endl;
        return 1;
    }
    
    int fd_m1 = gpio_m1.GetFileDescriptor();
    int fd_m2 = gpio_m2.GetFileDescriptor();
    
    if (fd_m1 < 0 || fd_m2 < 0) {
        std::cerr << "Failed to get file descriptors" << std::endl;
        return 1;
    }
    
    struct pollfd pfds[2];
    pfds[0].fd = fd_m1;
    pfds[0].events = POLLPRI | POLLERR;
    pfds[1].fd = fd_m2;
    pfds[1].events = POLLPRI | POLLERR;
    
    char buf[64];
    lseek(fd_m1, 0, SEEK_SET);
    read(fd_m1, buf, sizeof(buf));
    lseek(fd_m2, 0, SEEK_SET);
    read(fd_m2, buf, sizeof(buf));
    
    int m1_count = 0, m2_count = 0;
    struct timeval last_time, current_time;
    gettimeofday(&last_time, NULL);
    
    std::cout << "Monitoring both motors (Ctrl+C to stop)..." << std::endl;
    
    while (g_running) {
        int ret = poll(pfds, 2, 100);
        
        if (ret > 0) {
            if (pfds[0].revents & POLLPRI) {
                lseek(fd_m1, 0, SEEK_SET);
                read(fd_m1, buf, sizeof(buf));
                m1_count++;
            }
            if (pfds[1].revents & POLLPRI) {
                lseek(fd_m2, 0, SEEK_SET);
                read(fd_m2, buf, sizeof(buf));
                m2_count++;
            }
        }
        
        gettimeofday(&current_time, NULL);
        long elapsed_ms = (current_time.tv_sec - last_time.tv_sec) * 1000 +
                         (current_time.tv_usec - last_time.tv_usec) / 1000;
        
        if (elapsed_ms >= 200) {
            double m1_rate = (m1_count * 1000.0) / elapsed_ms;
            double m2_rate = (m2_count * 1000.0) / elapsed_ms;
            std::cout << "\rM1: " << std::fixed << std::setprecision(4) << std::setw(9) << m1_rate 
                      << " Hz  |  M2: " << std::setw(9) << m2_rate << " Hz   " << std::flush;
            m1_count = 0;
            m2_count = 0;
            last_time = current_time;
        }
    }
    
    std::cout << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "help") {
        print_help();
        return 0;
    }
    
    // M1 commands
    if (command == "m1") {
        if (argc < 3) {
            std::cerr << "Missing argument for m1 command" << std::endl;
            return 1;
        }
        
        std::string arg = argv[2];
        
        if (arg == "forward") {
            return cmd_m1_direction(true);
        } else if (arg == "reverse") {
            return cmd_m1_direction(false);
        } else if (arg == "monitor") {
            return cmd_m1_monitor();
        } else {
            // Try to parse as duty cycle
            try {
                int duty = std::stoi(arg);
                return cmd_m1_duty(duty);
            } catch (...) {
                std::cerr << "Invalid argument for m1: " << arg << std::endl;
                return 1;
            }
        }
    }
    
    // M2 commands
    if (command == "m2") {
        if (argc < 3) {
            std::cerr << "Missing argument for m2 command" << std::endl;
            return 1;
        }
        
        std::string arg = argv[2];
        
        if (arg == "forward") {
            return cmd_m2_direction(true);
        } else if (arg == "reverse") {
            return cmd_m2_direction(false);
        } else if (arg == "monitor") {
            return cmd_m2_monitor();
        } else {
            // Try to parse as duty cycle
            try {
                int duty = std::stoi(arg);
                return cmd_m2_duty(duty);
            } catch (...) {
                std::cerr << "Invalid argument for m2: " << arg << std::endl;
                return 1;
            }
        }
    }
    
    // Both motors monitor
    if (command == "m") {
        if (argc < 3) {
            std::cerr << "Missing argument for m command" << std::endl;
            return 1;
        }
        
        std::string arg = argv[2];
        if (arg == "monitor") {
            return cmd_both_monitor();
        } else {
            std::cerr << "Invalid argument for m: " << arg << std::endl;
            return 1;
        }
    }
    
    std::cerr << "Unknown command: " << command << std::endl;
    print_help();
    return 1;
}
