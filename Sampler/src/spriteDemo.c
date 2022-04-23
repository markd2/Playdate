#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct SpriteDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} SpriteDemo;


static int update(void *context)  {
    SpriteDemo *demo = (SpriteDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle sprite";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *spriteDemoSample(void) {
    SpriteDemo *demo = (SpriteDemo *)demoSampleNew("Sprite", kSprite,
                                                   update,
                                                   sizeof(SpriteDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    return (DemoSample *)demo;
} // drawingDemoSample


