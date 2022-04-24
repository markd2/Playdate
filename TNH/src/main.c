#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

ButtonPumper *pumper;

static const char *eventNames[] = {
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

static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (buttons == kButtonA && upDown == kPressed) {
        print("BUTTON A - STARTING");
    }
} // handleButtons


static int updateDisplay(void) {
    return 1;
} // updateDisplay


// --------------------------------------------------

// Return 1 to update the display, 0 to not update it
static int update(void *userdata) {
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(pumper, pushed, released);
    return updateDisplay();
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    // NOTHING GOES HERE
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        pd->display->setRefreshRate(2);
        LCDFont *font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        pd->system->setUpdateCallback(update, NULL);

        pumper = buttonPumperNew(handleButtons, NULL);

        break;
    }

    case kEventPause:
        break;

    case kEventResume:
        break;

    case kEventTerminate:
        break;

    default: break;
    }
    
    return 0;

} // eventHandler
