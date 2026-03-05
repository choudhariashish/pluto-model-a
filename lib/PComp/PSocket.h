#ifndef PSOCKET_H
#define PSOCKET_H

#include <string>
#include <functional>

namespace PSocket {

// Command structure for motor control
struct MotorCommand {
    enum Type {
        SET_SPEED,
        GET_STATUS,
        STOP,
        SHUTDOWN,
        INVALID
    };
    
    Type type;
    int motor;          // 1 or 2
    int speed;          // Target CPR
    int direction;      // 0=forward, 1=reverse
};

// Status structure for motor state
struct MotorStatus {
    int motor;
    int currentSpeed;   // Actual CPR
    int targetSpeed;    // Target CPR
    int direction;      // 0=forward, 1=reverse
    int dutyCycle;      // PWM duty cycle %
};

// Socket server for receiving commands
class Server {
public:
    Server(const char* socketPath);
    ~Server();
    
    // Start server in non-blocking mode
    bool start();
    
    // Check for incoming commands (non-blocking)
    // Returns true if command received
    bool checkCommand(MotorCommand& cmd);
    
    // Send status response
    void sendStatus(const MotorStatus& status);
    
    // Stop server
    void stop();
    
private:
    int serverFd;
    int clientFd;
    std::string socketPath;
    bool running;
};

// Socket client for sending commands
class Client {
public:
    Client(const char* socketPath);
    ~Client();
    
    // Connect to server
    bool connect();
    
    // Send command
    bool sendCommand(const MotorCommand& cmd);
    
    // Receive status
    bool receiveStatus(MotorStatus& status);
    
    // Disconnect
    void disconnect();
    
private:
    int sockFd;
    std::string socketPath;
    bool connected;
};

// Helper functions
MotorCommand parseCommand(const std::string& cmdStr);
std::string formatStatus(const MotorStatus& status);

} // namespace PSocket

#endif // PSOCKET_H
