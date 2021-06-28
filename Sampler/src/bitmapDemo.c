// Kitty art courtesy of publicdomainvectors.org

#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"
#include "patterns.h"

#include "pd_api.h"

typedef struct BitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;

    LCDBitmap *kitty;

    LCDFont *statusBarFont;
    char *statusMessage;
    int statusMessageLength;
    
} BitmapDemo;

// #defines instead of const ints because getting "initalizer not constant".
#define kStatusBarHeight 20
#define kQuadrantHeight (kScreenHeight / 2.0 - kStatusBarHeight / 2.0)

const Rect kTopLeftQuadrant = (Rect){
    0, 0,
    kScreenWidth / 2.0, kQuadrantHeight
};
const Rect kTopRightQuadrant = (Rect){
    kScreenWidth / 2.0, 0,
    kScreenWidth / 2.0, kQuadrantHeight
};
const Rect kBottomLeftQuadrant = (Rect){
    0, kQuadrantHeight,
    kScreenWidth / 2.0, kQuadrantHeight
};
const Rect kBottomRightQuadrant = (Rect){
    kScreenWidth / 2.0, kQuadrantHeight,
    kScreenWidth / 2.0, kQuadrantHeight
};


static void fillRect(Rect rect, LCDColor color) {
    pd->graphics->fillRect(rect.x, rect.y, rect.width, rect.height, color);
} // fillRect


static void frameRect(Rect rect, LCDColor color) {
    pd->graphics->drawRect(rect.x, rect.y, rect.width, rect.height, color);
} // fillRect


Rect centerSpanInRect(int width, int height, Rect rect) {
    int widthPad = (rect.width - width) / 2;
    int heightPad = (rect.height - height) / 2;

    rect.width = width;
    rect.height = height;
    rect.x += widthPad;
    rect.y += heightPad;

    return rect;
} // centerSpanInRect


static void drawShapes(BitmapDemo *demo) {
    fillRect(kTopLeftQuadrant, kColorBlack);
    fillRect(kTopRightQuadrant, kColorWhite);
    fillRect(kBottomLeftQuadrant, (LCDColor)percent50Pattern);
    fillRect(kBottomRightQuadrant, (LCDColor)bubblePattern);

    int kittyWidth, kittyHeight;
    int rowBytes;
    int hasMask;
    uint8_t *data;
    pd->graphics->getBitmapData(demo->kitty, &kittyWidth, &kittyHeight, &rowBytes, &hasMask, &data);

    // really want to print ascii
    // print("kitty width %d  height %d  rowBytes %d  hasMask %d",
    //       kittyWidth, kittyHeight, rowBytes, hasMask);

    Rect rect;

    pd->graphics->setDrawMode(kDrawModeNXOR);

    rect = centerSpanInRect(kittyWidth, kittyHeight, kTopLeftQuadrant);
    pd->graphics->drawBitmap(demo->kitty, rect.x, rect.y, kBitmapUnflipped);
    frameRect(rect, kColorWhite);

    rect = centerSpanInRect(kittyWidth, kittyHeight, kTopRightQuadrant);
    pd->graphics->drawBitmap(demo->kitty, rect.x, rect.y, kBitmapFlippedX);
    frameRect(rect, kColorBlack);

    // !!! interesting that these draw outside of the bounds - offset by 30someod
    rect = centerSpanInRect(kittyWidth, kittyHeight, kBottomLeftQuadrant);
    pd->graphics->drawBitmap(demo->kitty, rect.x, rect.y, kBitmapFlippedY);
    frameRect(rect, kColorBlack);

    rect = centerSpanInRect(kittyWidth, kittyHeight, kBottomRightQuadrant);
    pd->graphics->drawBitmap(demo->kitty, rect.x, rect.y, kBitmapFlippedXY);
    frameRect(rect, kColorWhite);

} // drawShapes


static void setStatusMessage(BitmapDemo *demo, char *message) {
    demo->statusMessage = message;
    demo->statusMessageLength = strlen(message);
} // setStatusMessage


static void drawStatusBar(BitmapDemo *demo) {
    pd->graphics->setFont(demo->statusBarFont);
    pd->graphics->drawText(demo->statusMessage, demo->statusMessageLength, kASCIIEncoding,
                           3, kScreenHeight - kStatusBarHeight + 3);

    int lineWidth = 2;
    pd->graphics->drawLine(0, kScreenHeight - kStatusBarHeight,
                           kScreenWidth, kScreenHeight - kStatusBarHeight, lineWidth, kColorBlack);
    
} // drawStatusBar


static int update(void *context)  {
    BitmapDemo *demo = (BitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle bitmap";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    drawShapes(demo);
    drawStatusBar(demo);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *bitmapDemoSample(void) {
    BitmapDemo *demo = (BitmapDemo *)demoSampleNew("Bitmap", kBitmap,
                                                     update,
                                                     sizeof(BitmapDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    const char *error;
    LCDBitmap *kitty = pd->graphics->loadBitmap("images/vector-kitty", &error);
    if (kitty == NULL) {
        print("could not load kitty image: %s", error);
    }
    demo->kitty = kitty;

    demo->statusBarFont = pd->graphics->loadFont("font/Roobert-11-Mono-Condensed", &error);
    if (demo->statusBarFont == NULL) {
        print("could not load font: %s", error);
    }

    setStatusMessage(demo, "chooby");

    return (DemoSample *)demo;
} // drawingDemoSample


