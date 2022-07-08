#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "geometry.h"
#include "drawhelpers.h"

#include "pd_api.h"

struct AudioDemoButton;
struct AudioDemo;

typedef void AudioDemoButtonCallback(struct AudioDemoButton *button,
                                     struct AudioDemo *demo);

typedef struct AudioDemoButton {
    const char *labelText;
    AudioDemoButtonCallback *callback;
    void *userdata;
} AudioDemoButton;

static AudioDemoButton buttonAlloc[16];
static LCDFont *buttonFont;

typedef struct AudioDemo {
    DemoSample isa;
    ButtonPumper *pumper;
    bool isDirty;
    AudioDemoButton *buttons;
    int audioDemoButtonCount;
    int audioDemoCurrentButtonIndex;
} AudioDemo;


void drawGrid(AudioDemo *demo) {
    // screen is a grid of 16 buttons, 4x4

    const int width = kScreenWidth / 4;
    const int height = kScreenHeight / 4;
    const int buttonMargin = 8;

    pd->graphics->setFont(buttonFont);

    for (int i = 0; i < demo->audioDemoButtonCount + 8; i++) {
        AudioDemoButton *button = &demo->buttons[i];

        int row = (i / 4);
        int column = (i % 4);
        Rect rect = (Rect){ width * row, height * column,
                            width, height };
        Rect inset = rectInset(rect, buttonMargin, buttonMargin);

        const char *labelText = button->labelText ?: "";

        if (i == demo->audioDemoCurrentButtonIndex) {
            fillRect(inset, kColorBlack);
            pd->graphics->pushContext(NULL); {
                pd->graphics->setDrawMode(kDrawModeInverted);
                drawCStringCenteredInRect(labelText, inset, buttonFont);
            } pd->graphics->popContext();
        } else {
            drawCStringCenteredInRect(labelText, inset, buttonFont);
            frameRect(inset, kColorBlack);
        }
    }
} // drawGrid


static int update(void *context)  {

    AudioDemo *demo = (AudioDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    if (!demo->isDirty) return 0;

    pd->graphics->clear(kColorWhite);

    drawGrid(demo);

    demo->isDirty = false;

    return 1;
} // update

void demoButtonPlaceholderCallback(AudioDemoButton *button,
                                   AudioDemo *demo) {
    print("button blah %s", button->userdata);
} // demoButtonPlaceholderCallback


typedef enum GridDirection {
    kGridDirectionUp,
    kGridDirectionDown,
    kGridDirectionLeft,
    kGridDirectionRight,
} GridDirection;


void triggerButton(AudioDemo *demo) {
    AudioDemoButton *button = &demo->buttons[demo->audioDemoCurrentButtonIndex];
    if (button != NULL && button->callback != NULL) {
        button->callback(button, demo);
    }
} // triggerButton


void moveButton(AudioDemo *sample, GridDirection direction) {
} // moveButton


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    AudioDemo *sample = (AudioDemo *)context;

    switch (buttons) {
    case kButtonUp:
        if (upDown == kPressed) {
            moveButton(sample, kGridDirectionUp);
        }
        break;
    case kButtonDown:
        if (upDown == kPressed) {
            moveButton(sample, kGridDirectionDown);
        }
        break;
    case kButtonLeft:
        if (upDown == kPressed) {
            moveButton(sample, kGridDirectionLeft);
        }
        break;
    case kButtonRight:
        if (upDown == kPressed) {
            moveButton(sample, kGridDirectionRight);
        }
        break;
    case kButtonA:
        if (upDown == kReleased) {
            triggerButton(sample);
        }
        break;
    case kButtonB:
        if (upDown == kReleased) {
            print("splunge");
        }
        break;
    }
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
    demo->audioDemoButtonCount = 16;
    demo->audioDemoCurrentButtonIndex = 2;

    for (int i = 0; i < 16; i++) {
        char buffer[128];
        snprintf(buffer, 128, "Button %d", i);
        demo->buttons[i].labelText = strdup(buffer);

        demo->buttons[i].callback = demoButtonPlaceholderCallback;
        demo->buttons[i].userdata = demo->buttons[i].labelText;
    }

    if (buttonFont == NULL) {
        const char *errorText = NULL;
        const char *fontpath = "font/Roobert-11-Mono-Condensed";
        buttonFont = pd->graphics->loadFont(fontpath, &errorText);
        if (buttonFont == NULL) {
            print("could not load button font %s - %s", fontpath, errorText);
        }
    }

    return (DemoSample *)demo;
} // drawingDemoSample


