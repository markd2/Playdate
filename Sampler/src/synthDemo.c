#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct SynthDemo {
    DemoSample isa;
    ButtonPumper *pumper;
} SynthDemo;


static int update(void *context)  {
    SynthDemo *demo = (SynthDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle synth";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *synthDemoSample(void) {
    SynthDemo *demo = (SynthDemo *)demoSampleNew("Synth", kSynth,
                                                 update,
                                                 sizeof(SynthDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    return (DemoSample *)demo;
} // drawingDemoSample


