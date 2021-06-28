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

static void drawShapes(void) {
    fillRect(kTopLeftQuadrant, kColorBlack);
    fillRect(kTopRightQuadrant, kColorWhite);
    fillRect(kBottomLeftQuadrant, kColorWhite);
    fillRect(kBottomRightQuadrant, kColorBlack);
} // drawShapes


static int update(void *context)  {
    BitmapDemo *demo = (BitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle bitmap";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    drawShapes();

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *bitmapDemoSample(void) {
    BitmapDemo *demo = (BitmapDemo *)demoSampleNew("Bitmap", kBitmap,
                                                     update,
                                                     sizeof(BitmapDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    return (DemoSample *)demo;
} // drawingDemoSample


