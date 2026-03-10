#include "PObject.h"

#include <sys/time.h>
#include <fstream>
#include <cstdarg>
#include <cstdio>
#include <syslog.h>
#include <signal.h>

PObject::LogDestination PObject::logDestination = PObject::PL_SYSLOG;

static int TimeoutValueUs = 0;
static void TimerCallback(int sig);

volatile uint64_t PObject::tickMs = 0;
volatile uint64_t PObject::tickPeriodMs = 0;
void PObject::timeout(void) { tickMs++; }
uint64_t PObject::getTickMs(void) { return tickMs; }
uint64_t PObject::getTickPeriodMs(void) { return tickPeriodMs; }

uint64_t PObject::getCurrentTimeMs(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

static void TimerCallback(int sig)
{
    struct itimerval timer;
    signal(SIGALRM, TimerCallback);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = TimeoutValueUs;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    PObject::timeout();
}

void PObject::setupTimeoutControl(uint32_t millisec)
{
    tickPeriodMs = millisec;
    TimeoutValueUs = 1000 * tickPeriodMs;
    TimerCallback(0);    // Kick off.
}

PObject::Status_t PObject::writeFile(const char* filename, const char* content)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << content;
        file.close();
        return Status_t::PL_OK;
    }
    return Status_t::PL_NOT_OK;
}

PObject::Status_t PObject::readFile(const char* filename, char* content, size_t size)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        file.read(content, size - 1);
        content[file.gcount()] = '\0';
        file.close();
        return Status_t::PL_OK;
    }
    return Status_t::PL_NOT_OK;
}

void PObject::Log(LogLevel level, const char* format, ...)
{
    const char* levelStr[] = {"ERROR", "WARN", "INFO", "DEBUG"};
    char buffer[1024];
    int len = snprintf(buffer, sizeof(buffer), "%s: ", levelStr[level]);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer + len, sizeof(buffer) - len, format, args);
    va_end(args);

    if (PObject::logDestination == PObject::PL_STDOUT || PObject::logDestination == PObject::PL_BOTH)
    {
        printf("%s\n", buffer);
    }

    if (PObject::logDestination == PObject::PL_SYSLOG || PObject::logDestination == PObject::PL_BOTH)
    {
        if (level == LogLevel::PL_LOG_DEBUG)
        {
            syslog(LOG_DEBUG, "%s", buffer);
        }
        else if (level == LogLevel::PL_LOG_INFO)
        {
            syslog(LOG_INFO, "%s", buffer);
        }
        else if (level == LogLevel::PL_LOG_WARN)
        {
            syslog(LOG_WARNING, "%s", buffer);
        }
        else if (level == LogLevel::PL_LOG_ERROR)
        {
            syslog(LOG_ERR, "%s", buffer);
        }
    }
}

