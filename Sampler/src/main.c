#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "spy.h"

#include "pd_api.h"

// prototypes of the sample-creation methods. Just so we won't have to have a header
// for each of them.
DemoSample *bitmapDemoSample(void);
DemoSample *moreBitmapDemoSample(void);
DemoSample *drawingDemoSample(void);
DemoSample *fontDemoSample(void);
DemoSample *tableDemoSample(void);
DemoSample *synthDemoSample(void);

// NULL-terminated array of known samples
DemoSample *allSamples[50];
int sampleCount;
int currentIndex;

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

void menuItemCallback(void *userdata) {
    int chosenOption = pd->system->getMenuItemValue(menuItem);
    int effectiveOption = (currentIndex + chosenOption) % sampleCount;

    if (effectiveOption != currentIndex) {
        selectDemo(effectiveOption);
    }

    pd->system->removeMenuItem(menuItem);
    menuItem = NULL;
} // menuItemCallback


static void setupMenu(void) {
    pd->system->removeAllMenuItems();

    const char *options[50] = { 0 };

    for (int i = 0; i < sampleCount; i++) {
        int biasedIndex = (currentIndex + i) % sampleCount;
        options[i] = allSamples[biasedIndex]->name;
    }

    menuItem = pd->system->addOptionsMenuItem("Demos", options, sampleCount,
                                              menuItemCallback, 
                                              "userdata (unused here)");
    
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
        DemoSample *moreBitmapSample = moreBitmapDemoSample();
        DemoSample *fontSample = fontDemoSample();
        DemoSample *tableSample = tableDemoSample();
        DemoSample *synthSample = synthDemoSample();

        allSamples[0] = drawingSample;
        allSamples[1] = bitmapSample;
        allSamples[2] = moreBitmapSample;
        allSamples[3] = fontSample;
        allSamples[4] = tableSample;
        allSamples[5] = synthSample;

        sampleCount = 6;

        selectDemo(2);

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
