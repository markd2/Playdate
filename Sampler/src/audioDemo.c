#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct AudioDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} AudioDemo;


static int update(void *context)  {
    AudioDemo *demo = (AudioDemo *)context;
    

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "Audo Demo(s)";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons

static const char *menuStringCallback(DemoSample *sample) {
    const char *menuText = "AUDIO DEMO\n"
        "* Does stuff with audio\n"
        "* And synth DAW stuffs";
    return menuText;
} // menuStringCallback


DemoSample *audioDemoSample(void) {
    AudioDemo *demo = (AudioDemo *)demoSampleNew("Audio", kAudio,
                                                 update,
                                                 sizeof(AudioDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);
    demo->isa.menuStringCallback = menuStringCallback;

    return (DemoSample *)demo;
} // drawingDemoSample


