// Kitty art courtesy of publicdomainvectors.org

#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"
#include "patterns.h"

#include "pd_api.h"

typedef struct DrawMode {
    char *name;
    LCDBitmapDrawMode drawMode;
} DrawMode;


static DrawMode drawModes[] = {
    { "Copy",             kDrawModeCopy },
    { "WhiteTransparent", kDrawModeWhiteTransparent },
    { "BlackTransparent", kDrawModeBlackTransparent },
    { "FillWhite",        kDrawModeFillWhite },
    { "FillBlack",        kDrawModeFillBlack },
    { "XOR",              kDrawModeXOR },
    { "NXOR",             kDrawModeNXOR },
    { "Inverted",         kDrawModeInverted }
};
static int kDrawModeCount = sizeof(drawModes) / sizeof(*drawModes);
static int currentDrawMode;


typedef struct BitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;

    LCDBitmap *kitty;

    int currentDrawModeIndex;

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


static void dumpBitmapAsASCII(int width, int height, 
                              int rowBytes, uint8_t *data) {
    char line[width + 1];

    line[width] = 0;

    uint8_t *rowScan;

    // kind of lazy, not properly handling a partial last byte.
    int stride = width / 8;

    for (int y = 0; y < height; y++) {
        uint8_t *rowScan = data + y * rowBytes;
        uint8_t *rowStop = rowScan + stride;

        char *lineScan = line;
        while (rowScan < rowStop) {
            for (int shift = 7; shift >= 0; shift--) {
                *lineScan++ = (*rowScan >> shift) & 0x01 ? '.' : '*';
            }

            rowScan++;
        }
        print(line);
    }
} // dump BitmapAsASCII


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

    pd->graphics->setDrawMode(drawModes[demo->currentDrawModeIndex].drawMode);

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


static void setDrawModeIndex(BitmapDemo *demo, int modeIndex) {
    DrawMode *mode = &drawModes[modeIndex];
    setStatusMessage(demo, mode->name);
    demo->currentDrawModeIndex = modeIndex;

} // setDrawModeIndex


static void drawStatusBar(BitmapDemo *demo) {
    pd->graphics->setDrawMode(kDrawModeCopy);

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

    pd->graphics->pushContext(NULL); {
        drawShapes(demo);
    } pd->graphics->popContext();

    drawStatusBar(demo);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    BitmapDemo *demo = (BitmapDemo *)context;

    // pump
    if (buttons == kButtonUp && upDown == kPressed) {
        int newIndex = demo->currentDrawModeIndex + 1;
        setDrawModeIndex(demo, newIndex % kDrawModeCount);
    }

    if (buttons == kButtonDown && upDown == kPressed) {
        int newIndex = demo->currentDrawModeIndex - 1;
        if (newIndex == -1) {
            newIndex = kDrawModeCount - 1;
        }
        setDrawModeIndex(demo, newIndex);
    }

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

    setDrawModeIndex(demo, 0);

    // Dump the image to the console - be sure to show it and embiggen.
    int kittyWidth, kittyHeight;
    int rowBytes;
    int hasMask;
    uint8_t *data;
    pd->graphics->getBitmapData(demo->kitty, &kittyWidth, &kittyHeight, &rowBytes, &hasMask, &data);

    dumpBitmapAsASCII(kittyWidth, kittyHeight, rowBytes, data);

    if (hasMask) {
        print("----");
        dumpBitmapAsASCII(kittyWidth, kittyHeight, rowBytes, data + rowBytes * kittyHeight);
    }
        

    return (DemoSample *)demo;
} // drawingDemoSample


