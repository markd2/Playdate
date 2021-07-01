#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct TilemapDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} TilemapDemo;


static int update(void *context)  {
    TilemapDemo *demo = (TilemapDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle tilemap";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *tilemapDemoSample(void) {
    TilemapDemo *demo = (TilemapDemo *)demoSampleNew("Tilemap", kBitmap,
                                                     update,
                                                     sizeof(TilemapDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    return (DemoSample *)demo;
} // drawingDemoSample


