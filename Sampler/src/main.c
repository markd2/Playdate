#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "spy.h"

#include "pd_api.h"

// prototypes of the sample-creation methods. Just so we won't have to have a header
// for each of them.
DemoSample *drawingDemoSample(void);
DemoSample *bitmapDemoSample(void);

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


void selectDemo(DemoSample *sample) {
    if (sample == currentSample) return;

    currentSample = sample;
    pd->system->setUpdateCallback(currentSample->updateCallback, currentSample);

} // selectDemo


PDMenuItem *menuItem;

// kinda lame the userdata isn't the userdate we set initially.
void menuItemCallback(void *userdata) {
    int chosenOption = pd->system->getMenuItemValue(menuItem);

    print("CHOOSENING %d", chosenOption);
    selectDemo(allSamples[chosenOption]);

    menuItem = NULL;
} // menuItemCallback


static void setupMenu(void) {
    pd->system->removeAllMenuItems();

    const char *options[50] = { 0 };
    DemoSample **scan, **stop;
    scan = allSamples;
    stop = scan + sizeof(allSamples) / sizeof(*allSamples);

    int count = 0;
    while (scan < stop) {
        if (*scan == NULL) break;
        options[count] = (*scan)->name;

        count++;
        scan++;
    }

    menuItem = pd->system->addOptionsMenuItem("Demos", options, count,
                                              menuItemCallback, "userdata?");
} // setupMenu


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        DemoSample *drawingSample = drawingDemoSample();
        DemoSample *bitmapSample = bitmapDemoSample();
        allSamples[0] = bitmapSample;
        allSamples[1] = drawingSample;

        selectDemo(allSamples[0]);

        pd->display->setRefreshRate(20);
        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        break;
    }

    case kEventPause:
        setupMenu();
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
