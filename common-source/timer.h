#ifndef BORK_TIMER_H
#define BORK_TIMER_H

#include <stdlib.h>


// --------------------------------------------------
// Support types

extern const int kMilliseconds;
typedef void TimerCallback(void *context);
typedef struct Timer Timer;


// --------------------------------------------------
// API

// Create a new timer.
// Give it a callback that'll get called when the timer fires.
// No guarantees of the order timers are called if they trigger at the same time.
// Turn off a timer by deleting it
//
Timer *timerNew(char *name, int msInterval, void *context, void (*callback)(void *context));

// Destroys a timer
void timerDelete(Timer *timer);

// Fires any timers that are due
void timerPump(int currentTime);


// --------------------------------------------------
// innards

struct Timer {
    char *name;
    int msInterval;
    int lastFire;

    void *context;
    void (*callback)(void *context);
};


#endif // BORK_TIMER_H
