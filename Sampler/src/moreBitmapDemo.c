#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct MoreBitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} MoreBitmapDemo;


static int update(void *context)  {
    MoreBitmapDemo *demo = (MoreBitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "Bitmap 2";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *moreBitmapDemoSample(void) {
    MoreBitmapDemo *demo = (MoreBitmapDemo *)demoSampleNew("Bitmap 2", kBitmap,
                                                           update,
                                                           sizeof(MoreBitmapDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    return (DemoSample *)demo;
} // drawingDemoSample


