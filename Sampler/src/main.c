#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "spy.h"

#include "pd_api.h"

// prototypes of the sample-creation methods. Just so we won't have to have a header
// for each of them.
DemoSample *drawingDemoSample(void);

// NULL-terminated array of known samples
DemoSample *allSamples[50];
DemoSample *currentSample;


static LCDFont *font;

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


static int update(void *userdata) {
    return 1;
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit:
        currentSample = drawingDemoSample();
        allSamples[0] = currentSample;

        pd->system->setUpdateCallback(currentSample->updateCallback, currentSample);

        pd->display->setRefreshRate(20);
        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        break;

    case kEventPause:
//        setupMenu();
        break;

    case kEventResume:
        print("hey!  back from menu!");
        break;

    case kEventTerminate:
        uninstallSpies();
        break;

    default: break;
    }
    
    return 0;

} // eventHandler
