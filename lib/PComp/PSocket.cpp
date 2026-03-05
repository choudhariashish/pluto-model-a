#include "PSocket.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <iostream>

namespace PSocket {

Server::Server(const char* path) 
    : serverFd(-1), clientFd(-1), socketPath(path), running(false) {
}

Server::~Server() {
    stop();
}

bool Server::start() {
    // Remove existing socket file
    unlink(socketPath.c_str());
    
    // Create socket
    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("socket");
        return false;
    }
    
    // Set non-blocking
    int flags = fcntl(serverFd, F_GETFL, 0);
    fcntl(serverFd, F_SETFL, flags | O_NONBLOCK);
    
    // Bind socket
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    
    if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(serverFd);
        return false;
    }
    
    // Listen
    if (listen(serverFd, 1) < 0) {
        perror("listen");
        close(serverFd);
        return false;
    }
    
    running = true;
    return true;
}

bool Server::checkCommand(MotorCommand& cmd) {
    if (!running) return false;
    
    // Accept new connection if no client connected
    if (clientFd < 0) {
        clientFd = accept(serverFd, NULL, NULL);
        if (clientFd < 0) {
            return false; // No connection yet
        }
        // Set client socket non-blocking
        int flags = fcntl(clientFd, F_GETFL, 0);
        fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
    }
    
    // Try to read command
    ssize_t n = read(clientFd, &cmd, sizeof(cmd));
    if (n == sizeof(cmd)) {
        return true;
    } else if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        // Client disconnected or error
        close(clientFd);
        clientFd = -1;
    }
    
    return false;
}

void Server::sendStatus(const MotorStatus& status) {
    if (clientFd >= 0) {
        write(clientFd, &status, sizeof(status));
    }
}

void Server::stop() {
    if (clientFd >= 0) {
        close(clientFd);
        clientFd = -1;
    }
    if (serverFd >= 0) {
        close(serverFd);
        serverFd = -1;
    }
    unlink(socketPath.c_str());
    running = false;
}

Client::Client(const char* path) 
    : sockFd(-1), socketPath(path), connected(false) {
}

Client::~Client() {
    disconnect();
}

bool Client::connect() {
    sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket");
        return false;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    
    if (::connect(sockFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockFd);
        sockFd = -1;
        return false;
    }
    
    connected = true;
    return true;
}

bool Client::sendCommand(const MotorCommand& cmd) {
    if (!connected) return false;
    
    ssize_t n = write(sockFd, &cmd, sizeof(cmd));
    return (n == sizeof(cmd));
}

bool Client::receiveStatus(MotorStatus& status) {
    if (!connected) return false;
    
    ssize_t n = read(sockFd, &status, sizeof(status));
    return (n == sizeof(status));
}

void Client::disconnect() {
    if (sockFd >= 0) {
        close(sockFd);
        sockFd = -1;
    }
    connected = false;
}

MotorCommand parseCommand(const std::string& cmdStr) {
    MotorCommand cmd;
    cmd.type = MotorCommand::INVALID;
    
    std::istringstream iss(cmdStr);
    std::string action, motor, dir;
    
    iss >> action;
    
    if (action == "set") {
        iss >> motor >> cmd.speed >> dir;
        cmd.type = MotorCommand::SET_SPEED;
        cmd.motor = (motor == "m1") ? 1 : 2;
        cmd.direction = (dir == "forward") ? 0 : 1;
    } else if (action == "status") {
        iss >> motor;
        cmd.type = MotorCommand::GET_STATUS;
        if (motor == "m1") cmd.motor = 1;
        else if (motor == "m2") cmd.motor = 2;
        else cmd.motor = 0; // all
    } else if (action == "stop") {
        iss >> motor;
        cmd.type = MotorCommand::STOP;
        cmd.motor = (motor == "m1") ? 1 : 2;
    } else if (action == "shutdown") {
        cmd.type = MotorCommand::SHUTDOWN;
    }
    
    return cmd;
}

std::string formatStatus(const MotorStatus& status) {
    std::ostringstream oss;
    oss << "{\"motor\":\"m" << status.motor << "\""
        << ",\"current_speed\":" << status.currentSpeed
        << ",\"target_speed\":" << status.targetSpeed
        << ",\"direction\":\"" << (status.direction == 0 ? "forward" : "reverse") << "\""
        << ",\"duty_cycle\":" << status.dutyCycle
        << "}";
    return oss.str();
}

} // namespace PSocket
