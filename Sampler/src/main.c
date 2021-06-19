#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "spy.h"

#include "pd_api.h"

DemoSample *drawingDemoSample(void);


DemoSample *currentSample;

static LCDFont *font;

static const int kScreenWidth = LCD_COLUMNS;
static const int kScreenHeight = LCD_ROWS;

static const int kTextWidth = 86;
static const int kTextHeight = 16;

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
    if (pd == NULL) {
        pd = playdate;
//        installSpies();
    }

    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit:
        currentSample = drawingDemoSample();
        pd->system->setUpdateCallback(currentSample->updateCallback, NULL);

        pd->display->setRefreshRate(20);
        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);
        break;

    case kEventInitLua:
        break;

    case kEventLock:
        break;

    case kEventUnlock:
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

    case kEventKeyPressed: // arg is keycode
        break;

    case kEventKeyReleased:
        break;

    case kEventLowPower:
        break;
    }
    
    return 0;

} // eventHandler
