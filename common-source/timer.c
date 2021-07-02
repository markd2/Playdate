#include "timer.h"

#include "memory.h"

const int kMilliseconds = 1000;

Timer *splunge;


Timer *timerNew(char *name, int msInterval, void *context, void (*callback)(void *context)) {
    Timer *timer = pdMalloc(sizeof(Timer));
    timer->name = name;
    timer->msInterval = msInterval;
    timer->lastFire = 0;
    timer->context = context;
    timer->callback = callback;

    // add to global pool of timers, and have a single pumper

    splunge = timer;

    return timer;
} // timerNew


void timerDelete(Timer *timer) {
    pdFree(timer);
} // timerDelegate


void timerPump(int currentTime) {
    Timer *timer = splunge;

    int nextFire = timer->lastFire + timer->msInterval;
    if (nextFire < currentTime) {
        timer->callback(timer->context);
        timer->lastFire = currentTime;
    }

} // timerPump

