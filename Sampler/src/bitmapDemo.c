#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct BitmapDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} BitmapDemo;


static int update(void *context)  {
    BitmapDemo *demo = (BitmapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

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


