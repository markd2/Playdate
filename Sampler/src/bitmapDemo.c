// Kitty art courtesy of publicdomainvectors.org

#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"

#include "pd_api.h"

typedef struct BitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;

    LCDBitmap *kitty;
    
} BitmapDemo;


const Rect kTopLeftQuadrant = (Rect){
    0, 0,
    kScreenWidth / 2.0, kScreenHeight / 2.0
};
const Rect kTopRightQuadrant = (Rect){
    kScreenWidth / 2.0, 0,
    kScreenWidth / 2.0, kScreenHeight / 2.0
};
const Rect kBottomLeftQuadrant = (Rect){
    0, kScreenHeight / 2.0,
    kScreenWidth / 2.0, kScreenHeight / 2.0
};
const Rect kBottomRightQuadrant = (Rect){
    kScreenWidth / 2.0, kScreenHeight / 2.0,
    kScreenWidth / 2.0, kScreenHeight / 2.0
};


static void fillRect(Rect rect, LCDColor color) {
    pd->graphics->fillRect(rect.x, rect.y, rect.width, rect.height, color);
} // fillRect


Rect centerSpanInRect(int width, int height, Rect rect) {
    return rect;
} // centerSpanInRect


static void drawShapes(BitmapDemo *demo) {
    fillRect(kTopLeftQuadrant, kColorBlack);
    fillRect(kTopRightQuadrant, kColorWhite);
    fillRect(kBottomLeftQuadrant, kColorWhite);
    fillRect(kBottomRightQuadrant, kColorBlack);

    int kittyWidth, kittyHeight;
    int rowBytes;
    int hasMask;
    uint8_t *data;
    pd->graphics->getBitmapData(demo->kitty, &kittyWidth, &kittyHeight, &rowBytes, &hasMask, &data);
    print("kitty width %d  height %d  rowBytes %d  hasMask %d",
          kittyWidth, kittyHeight, rowBytes, hasMask);

    Rect rect;

    rect = centerSpanInRect(kittyWidth, kittyHeight, kTopLeftQuadrant);
    int flip = 0;
    pd->graphics->drawBitmap(demo->kitty, rect.x, rect.y, flip);

} // drawShapes


static int update(void *context)  {
    BitmapDemo *demo = (BitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle bitmap";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    drawShapes(demo);

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

    return (DemoSample *)demo;
} // drawingDemoSample


