#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "timing.h"

#include "pd_api.h"

// prototypes of the sample-creation methods. Just so we won't have to have a header
// for each of them.
DemoSample *bitmapDemoSample(void);
DemoSample *moreBitmapDemoSample(void);
DemoSample *drawingDemoSample(void);
DemoSample *spriteDemoSample(void);

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
    // NOTHING GOES HERE
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    float blah = 3.1415;
    volatile float constant = 0.1234;

    print("sizeof %d vs %d", sizeof(float), sizeof(double));

    TIMING_START {
        for (volatile int i = 0; i < 10000000; i++) {
            blah += constant;
        }
    } TIMING_END;

    pd->display->setMosaic(1, 1);

    switch (event) {
    case kEventInit: {
        DemoSample *drawingSample = drawingDemoSample();
        DemoSample *bitmapSample = bitmapDemoSample();
        DemoSample *moreBitmapSample = moreBitmapDemoSample();
        DemoSample *spriteSample = spriteDemoSample();
        DemoSample *fontSample = fontDemoSample();
        DemoSample *tableSample = tableDemoSample();
        DemoSample *synthSample = synthDemoSample();

        allSamples[0] = drawingSample;
        allSamples[1] = bitmapSample;
        allSamples[2] = moreBitmapSample;
        allSamples[3] = spriteSample;
        allSamples[4] = fontSample;
        allSamples[5] = tableSample;
        allSamples[6] = synthSample;

        sampleCount = 7;

        selectDemo(3);

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
        break;

    default: break;
    }
    
    return 0;

} // eventHandler
