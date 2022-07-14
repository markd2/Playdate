#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI* pd;
#define print pd->system->logToConsole

static int update(void *userdata) {
    pd->graphics->clear(kColorWhite);

    int x = 10;
    int y = 30;
    int width = 20;
    int height = 40;
    pd->graphics->drawRect(x, y, width, height, kColorBlack);
    
    return 1;
} // update


#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    switch (event) {
    case kEventInit:
        pd = playdate;
        print("kEventInit");
        // setting this now assumes pure C ad doesn't run any Lua code
        pd->system->setUpdateCallback(update, NULL);
        break;

    case kEventInitLua:
        print("kEventInitLua");
        break;

    case kEventLock:
        print("kEventLock");
        break;

    case kEventUnlock:
        print("kEventUnlock");
        break;

    case kEventPause:
        print("kEventPause");
        break;

    case kEventResume:
        print("kEventResume");
        break;

    case kEventTerminate:
        print("kEventTerminate");
        break;

    case kEventKeyPressed:
        print("kEventKeyPressed");
        break;

    case kEventKeyReleased:
        print("kEventKeyReleased");
        break;

    case kEventLowPower:
        print("kEventLowPower");
        break;
    }

    return 0;
} // eventHandler
