#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"
#include "patterns.h"
#include "memory.h"

#include "pd_api.h"

const float minScaleX = 0.5;
const float maxScaleX = 2.0;
const float minScaleY = 0.2;
const float maxScaleY = 3.0;


// ----------

static int kMilliseconds = 1000;

typedef struct Timer {
    char *name;
    int msInterval;
    int lastFire;

    void *context;
    void (*callback)(void *context);
} Timer;


Timer *timerNew(char *name, int msInterval, void *context, void (*callback)(void *context)) {
    Timer *timer = pdMalloc(sizeof(Timer));
    timer->name = name;
    timer->msInterval = msInterval;
    timer->lastFire = 0;
    timer->context = context;
    timer->callback = callback;

    // add to global pool of timers, and have a single pumper

    return timer;
} // timerNew


void timerDelete(Timer *timer) {
    pdFree(timer);
} // timerDelegate


void timerPump(Timer *timer, int currentTime) {
    int nextFire = timer->lastFire + timer->msInterval;
    if (nextFire < currentTime) {
        timer->callback(timer->context);
        timer->lastFire = currentTime;
    }
} // timerPump

// ----------




typedef struct MoreBitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;

    LCDBitmap *worldImage;
    float worldScaleX;
    float worldScaleXIncrement;
    float worldScaleY;
    float worldScaleYIncrement;
    LCDBitmap *scaledWorldLabel;
    Timer *worldScaleTimer;

    LCDBitmap *circuitImage;
} MoreBitmapDemo;


static void drawShapes(MoreBitmapDemo *demo) {
    pd->graphics->drawScaledBitmap(demo->worldImage, 13, 13,
                                   demo->worldScaleX, demo->worldScaleY);
    pd->graphics->drawScaledBitmap(demo->scaledWorldLabel, 0, 0,
                                   demo->worldScaleX, 1.0);
    pd->graphics->drawBitmap(demo->circuitImage, 50, 50, kBitmapUnflipped);
} // drawShapes


static void mungeShapes(MoreBitmapDemo *demo) {
    demo->worldScaleX += demo->worldScaleXIncrement;;
    if (demo->worldScaleX > maxScaleX || demo->worldScaleX < minScaleX) {
        demo->worldScaleXIncrement *= -1;
    }
    demo->worldScaleY += demo->worldScaleYIncrement;
    if (demo->worldScaleY > maxScaleY || demo->worldScaleY < minScaleY) {
        demo->worldScaleYIncrement *= -1;
    }

} // mungeShapes


static void mungeTimer(void *context) {
    MoreBitmapDemo *demo = context;
    mungeShapes(demo);
} // mungeTimer


static int update(void *context)  {
    MoreBitmapDemo *demo = (MoreBitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    timerPump(demo->worldScaleTimer, pd->system->getCurrentTimeMilliseconds());

//    mungeShapes(demo);

    drawShapes(demo);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


static LCDBitmap *makeScaledWorldLabel(void) {
    const char *text = "scaled";
    int textlen = strlen(text);

    const char *errorText;
    LCDFont *font = pd->graphics->loadFont("font/Sasser-Small-Caps", &errorText);
    if (font == NULL) {
        print("could not load font. %s", errorText);
        return NULL;
    }

    int tracking = 5;
    int width = pd->graphics->getTextWidth(font, text, textlen, kASCIIEncoding, tracking);
    width -= (tracking * 2.5); // getting extra space at the end when drawing
    int height = 16;
    
    LCDBitmap *bitmap = pd->graphics->newBitmap(width, height, kColorBlack);
    
    pd->graphics->pushContext(bitmap); {
        pd->graphics->setDrawMode(kDrawModeFillWhite);
        pd->graphics->setTextTracking(tracking);
        pd->graphics->drawText(text, textlen, kASCIIEncoding, 0, 0);
    } pd->graphics->popContext();

    return bitmap;

} // makeScaledWorldLabel


DemoSample *moreBitmapDemoSample(void) {
    MoreBitmapDemo *demo = (MoreBitmapDemo *)demoSampleNew("Bitmap 2", kBitmap,
                                                           update,
                                                           sizeof(MoreBitmapDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    const char *error;

    LCDBitmap *world = pd->graphics->loadBitmap("images/world", &error);
    if (world == NULL) {
        print("could not load world image: %s", error);
    }
    demo->worldImage = world;
    demo->worldScaleX = 1.0;
    demo->worldScaleXIncrement = 0.1;
    demo->worldScaleY = 1.0;
    demo->worldScaleYIncrement = 0.05;
    demo->scaledWorldLabel = makeScaledWorldLabel();

    LCDBitmap *circuit = pd->graphics->loadBitmap("images/circuit-pad", &error);
    if (circuit == NULL) {
        print("could not load circuit image: %s", error);
    }
    demo->circuitImage = circuit;

    demo->worldScaleTimer = timerNew("world timer", (1.0 / 10.0) * kMilliseconds, demo, mungeTimer);

    return (DemoSample *)demo;
} // drawingDemoSample


