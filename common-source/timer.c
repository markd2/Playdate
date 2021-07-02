#include "timer.h"
#include "globals.h"

#include "memory.h"

const int kMilliseconds = 1000;

#define kMaxTimers 50
static Timer *timers[kMaxTimers];

static int timerCount;

Timer *timerNew(char *name, int msInterval, void *context, void (*callback)(void *context)) {
    if (timerCount == kMaxTimers) {
        print("too many timers. Yay static buffers");
        int *blah = (int *)0x1;  *blah = 5;
    }

    Timer *timer = pdMalloc(sizeof(Timer));
    timer->name = name;
    timer->msInterval = msInterval;
    timer->lastFire = 0;
    timer->context = context;
    timer->callback = callback;

    timers[timerCount++] = timer;

    return timer;
} // timerNew


static int indexForTimer(Timer *timer) {
    for (int i = 0; i < timerCount; i++) {
        if (timer == timers[i]) {
            return i;
        }
    }
    return -1;
} // indexForTimer


void timerDelete(Timer *timer) {
    int index = indexForTimer(timer);
    if (index == -1) {
        print("not finding a timer when deleting");
        return;
    }
    pdFree(timer);

    timerCount--;
    timers[index] = timers[timerCount];

} // timerDelegate


void timerPump(int currentTime) {
    for (int i = 0; i < timerCount; i++) {
        Timer *timer = timers[i];

        int nextFire = timer->lastFire + timer->msInterval;
        if (nextFire < currentTime) {
            timer->callback(timer->context);
            timer->lastFire = currentTime;
        }
    }

} // timerPump

