
#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "globals.h"
#include "timing.h"
#include "drawhelpers.h"

#include "pd_api.h"
#include "stb_ds.h"

// prototypes of the sample-creation methods. Just so we won't have to have a header
// for each of them.
DemoSample *bitmapDemoSample(void);
DemoSample *moreBitmapDemoSample(void);
DemoSample *drawingDemoSample(void);
DemoSample *spriteDemoSample(void);

DemoSample *fontDemoSample(void);
DemoSample *tableDemoSample(void);
DemoSample *audioDemoSample(void);

// NULL-terminated array of known samples
DemoSample *allSamples[50];
int sampleCount;
int currentIndex;

static LCDFont *font;


// Return 1 to update the display, 0 to not update it
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


LCDBitmap *menuImageBitmap(const char *menuText) {
    static LCDBitmap *menuImageBitmap;
    static LCDFont *menuImageFont;
    static WordWidthHash *menuImageWordWidthHash;
    
    if (menuImageBitmap == NULL) {
        menuImageBitmap = pd->graphics->newBitmap(kScreenWidth, kScreenHeight, kColorWhite);

        const char *errorText = NULL;
        const char *fontpath = "font/Roobert-11-Mono-Condensed";
        menuImageFont = pd->graphics->loadFont(fontpath, &errorText);
        if (menuImageFont == NULL) {
            print("could not load font %s - %s", fontpath, errorText);
        }
        sh_new_arena(menuImageWordWidthHash);
        
        // Make sure an initial allocation for the hash table is made
        // before passing it around.
        shput(menuImageWordWidthHash, "prime", 0);
    }

    Rect rect = (Rect){ kScreenWidth / 2, 0, 
                        kScreenWidth / 2, kScreenHeight };
    Rect insetRect = rectInset(rect, 10, 10);
    pd->graphics->pushContext(menuImageBitmap); {
//        demoView->isDirty = true;
//        demoView->updateCallback(demoView);
        
        fillRect(rect, kColorWhite);
        drawWrappedString(menuText,
                          menuImageFont, insetRect,
                          &menuImageWordWidthHash,
                          6);
        
    } pd->graphics->popContext();

    return menuImageBitmap;

} // menuImageBitmap

// TODO - add a way to get the menu string callback to arrange a rendering
// of the current screen so the menu coming on-screen is cooler.
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

    DemoSample *currentSample = allSamples[currentIndex];
    if (currentSample->menuStringCallback == NULL) {
        pd->system->setMenuImage(NULL, 0);
    } else {
        const char *menuString = currentSample->menuStringCallback(currentSample);
        if (menuString != NULL) {
            LCDBitmap *bitmap = menuImageBitmap(menuString);
            pd->system->setMenuImage(bitmap, kScreenWidth / 2);
        }
    }
    
} // setupMenu


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    // NOTHING GOES HERE
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        DemoSample *drawingSample = drawingDemoSample();
        DemoSample *bitmapSample = bitmapDemoSample();
        DemoSample *moreBitmapSample = moreBitmapDemoSample();
        DemoSample *spriteSample = spriteDemoSample();
        DemoSample *fontSample = fontDemoSample();
        DemoSample *tableSample = tableDemoSample();
        DemoSample *audioSample = audioDemoSample();

        allSamples[0] = drawingSample;
        allSamples[1] = bitmapSample;
        allSamples[2] = moreBitmapSample;
        allSamples[3] = spriteSample;
        allSamples[4] = fontSample;
        allSamples[5] = tableSample;
        allSamples[6] = audioSample;

        sampleCount = 7;

        selectDemo(4);

        pd->display->setRefreshRate(60);
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
