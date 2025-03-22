#include "PObject.h"

#include <sys/time.h>

static int TimeoutValueUs = 0;
static void TimerCallback(int sig);

volatile uint64_t PObject::tickMs = 0;
volatile uint64_t PObject::tickPeriodMs = 0;
void PObject::timeout(void) { tickMs++; }
uint64_t PObject::getTickMs(void) { return tickMs; }
uint64_t PObject::getTickPeriodMs(void) { return tickPeriodMs; }

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