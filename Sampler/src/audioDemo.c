#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

struct AudioDemoButton;
typedef void AudioDemoButtonCallback(struct AudioDemoButton *button);

typedef struct AudioDemoButton {
    const char *labelText;
    AudioDemoButtonCallback *callback;
    void *userdata;
} AudioDemoButton;

static AudioDemoButton buttonAlloc[16];

typedef struct AudioDemo {
    DemoSample isa;
    ButtonPumper *pumper;
    bool isDirty;
    AudioDemoButton *buttons;
    int audioDemoButtonCount;
    int audioDemoCurrentButton;
} AudioDemo;


void drawGrid(AudioDemo *demo) {
} // drawGrid


static int update(void *context)  {

    AudioDemo *demo = (AudioDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    if (!demo->isDirty) return 0;

    pd->graphics->clear(kColorWhite);

    drawGrid(demo);

    return 1;
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    print("YAY");
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
    demo->isDirty = true;
    demo->isa.menuStringCallback = menuStringCallback;

    demo->buttons = buttonAlloc;
    demo->audioDemoButtonCount = 8;

    for (int i = 0; i < 8; i++) {
        char buffer[128];
        snprintf(buffer, 128, "Button %d", i);
        demo->buttons[i].labelText = strdup(buffer);
    }

    return (DemoSample *)demo;
} // drawingDemoSample


