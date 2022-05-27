#include "globals.h"

PlaydateAPI *pd;

const char *eventNames[] = {
    "kEventInit",
    "kEventInitLua",
    "kEventLock",
    "kEventUnlock",
    "kEventPause",
    "kEventResume",
    "kEventTerminate",
    "kEventKeyPressed", // arg is keycode
    "kEventKeyReleased",
    "kEventLowPower"
};
