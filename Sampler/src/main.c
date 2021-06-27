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
DemoSample *fontDemoSample(void);
DemoSample *tableDemoSample(void);
DemoSample *synthDemoSample(void);

// NULL-terminated array of known samples
DemoSample *allSamples[50];
int currentIndex = 0;


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


void selectDemo(int sampleIndex) {
    DemoSample *currentSample = allSamples[sampleIndex];
    pd->system->setUpdateCallback(currentSample->updateCallback, currentSample);
    currentIndex = sampleIndex;
} // selectDemo


PDMenuItem *menuItem;

// kinda lame the userdata isn't the userdate we set initially.
void menuItemCallback(void *userdata) {
    int chosenOption = pd->system->getMenuItemValue(menuItem);

    if (chosenOption != currentIndex) {
        selectDemo(chosenOption);
    }

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
        DemoSample *fontSample = fontDemoSample();
        DemoSample *tableSample = tableDemoSample();
        DemoSample *synthSample = synthDemoSample();

        allSamples[0] = bitmapSample;
        allSamples[1] = drawingSample;
        allSamples[2] = fontSample;
        allSamples[3] = tableSample;
        allSamples[4] = synthSample;

        selectDemo(0);

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
