#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"
#include "patterns.h"
#include "memory.h"
#include "timer.h"

#include "pd_api.h"

const float minScaleX = 0.5;
const float maxScaleX = 2.0;
const float minScaleY = 0.2;
const float maxScaleY = 3.0;

typedef struct Bouncer Bouncer;

#define kBouncerCount 10

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

    Bouncer *bouncers[kBouncerCount];

} MoreBitmapDemo;

// ----------

typedef struct Bouncer {
    LCDBitmap *image;
    int x;
    int y;
    int xIncrement;
    int yIncrement;

    int width;
    int height;

    bool colliding;

    Timer *timer;
} Bouncer;

static void moveBouncer(void *context);

// Assumes bitmap is going to stick around.
static Bouncer *bouncerNew(LCDBitmap *bitmap, int x, int y, int xIncrement, int yIncrement,
                           int timerIntervalMs) {
    Bouncer *bouncer = pdMalloc(sizeof(Bouncer));

    bouncer->image = bitmap;
    bouncer->x = x;
    bouncer->y = y;
    bouncer->xIncrement = xIncrement;
    bouncer->yIncrement = yIncrement;

    pd->graphics->getBitmapData(bitmap, &bouncer->width, &bouncer->height,
                                NULL, NULL, NULL);
    bouncer->colliding = false;

    bouncer->timer = timerNew("bouncer", timerIntervalMs, bouncer, moveBouncer);
    return bouncer;
} // bouncerNew



static void bouncerDelete(Bouncer *bouncer) {
    timerDelete(bouncer->timer);
    pdFree(bouncer);
} // bouncerDelete


static Rect bouncerRect(Bouncer *bouncer) {
    return (Rect){ bouncer->x, bouncer->y, bouncer->width, bouncer->height };
} // bouncerRect


static void bouncerMove(Bouncer *bouncer) {
    bouncer->x += bouncer->xIncrement;
    bouncer->y += bouncer->yIncrement;

    if (bouncer->x < 0) {
        bouncer->x = 0;
        bouncer->xIncrement *= -1;
    }

    if (bouncer->x + bouncer->width > kScreenWidth) {
        bouncer->x = kScreenWidth - bouncer->width;
        bouncer->xIncrement *= -1;
    }
    
    if (bouncer->y < 0) {
        bouncer->y = 0;
        bouncer->yIncrement *= -1;
    }

    if (bouncer->y + bouncer->height > kScreenHeight) {
        bouncer->y = kScreenHeight - bouncer->height;
        bouncer->yIncrement *= -1;
    }

} // bouncerMove


static void moveBouncer(void *context) {
    Bouncer *bouncer = context;
    bouncerMove(bouncer);
} // moveBouncer



// ----------


static void drawShapes(MoreBitmapDemo *demo) {
    pd->graphics->drawScaledBitmap(demo->worldImage, 13, 13,
                                   demo->worldScaleX, demo->worldScaleY);
    pd->graphics->drawScaledBitmap(demo->scaledWorldLabel, 0, 0,
                                   demo->worldScaleX, 1.0);

    for (int i = 0; i < kBouncerCount; i++) {
        Bouncer *bouncer = demo->bouncers[i];
        pd->graphics->drawBitmap(bouncer->image, bouncer->x, bouncer->y, kBitmapUnflipped);
        if (bouncer->colliding) {
            frameRect(bouncerRect(bouncer), kColorBlack);
        }
    }
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


// totally naive O(N^2) comparisons
static void checkCollisions(MoreBitmapDemo *demo) {
    for (int i = 0; i < kBouncerCount; i++) {
        demo->bouncers[i]->colliding = false;
    }

    for (int i = 0; i < kBouncerCount; i++) {
        Bouncer *anchor = demo->bouncers[i];
        Rect anchorRect = bouncerRect(anchor);

        for (int j = 0; j < kBouncerCount; j++) {
            if (i == j) continue;

            Bouncer *inner = demo->bouncers[j];
            if (inner->colliding) continue;

            // first see if rects intersect
            Rect innerRect = bouncerRect(inner);
            if (rectsIntersect(anchorRect, innerRect)) {
                anchor->colliding = true;
                inner->colliding = true;
                break;
            }
        }
    }
    
} // checkCollisions


static int update(void *context)  {
    MoreBitmapDemo *demo = (MoreBitmapDemo *)context;

    checkCollisions(demo);

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    timerPump(pd->system->getCurrentTimeMilliseconds());

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

    demo->worldScaleTimer = timerNew("world timer", (1.0 / 10.0) * kMilliseconds, demo, mungeTimer);

    LCDBitmap *circuit = pd->graphics->loadBitmap("images/circuit-pad", &error);
    if (circuit == NULL) {
        print("could not load circuit image: %s", error);
    }

    int circuitWidth;
    int circuitHeight;
    pd->graphics->getBitmapData(circuit, &circuitWidth, &circuitHeight, NULL, NULL, NULL);

    for (int i = 0; i < kBouncerCount; i++) {
        int x = rand() % (kScreenWidth - circuitWidth);
        int y = rand() % (kScreenHeight - circuitHeight);

        int xSign = rand() % 2 ? -1 : 1;
        int ySign = rand() % 2 ? -1 : 1;
        
        int xSpeed = rand() % 3 + 1;
        int ySpeed = rand() % 4 + 1;

        float timerSpeed = (rand() % 10) / 20.0;
    
        Bouncer *bouncer = bouncerNew(circuit, x, y, xSpeed * xSign, ySpeed * ySign,
                                      timerSpeed * kMilliseconds);
        demo->bouncers[i] = bouncer;
    }

    return (DemoSample *)demo;
} // drawingDemoSample


