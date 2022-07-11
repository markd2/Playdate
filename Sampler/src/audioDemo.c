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
    struct AudioDemo *demo;
    void *userdata;
} AudioDemoButton;


/* the usual 
 *     0     1     2
 *  0  +-0-+ +-1-+ +-2-+
 *  1  +-3-+ +-4-+ +-5-+
 *  2  +-6-+ +-7-+ +-8-+
 *  3  +-9-+ +-10+ +-11+
 */


#define kGridRows 4
#define kGridColumns 3

static AudioDemoButton buttonAlloc[kGridRows * kGridColumns];
static LCDFont *buttonFont;

typedef struct AudioDemo {
    DemoSample isa;
    ButtonPumper *pumper;
    bool isDirty;
    AudioDemoButton *buttons;
    int audioDemoButtonCount;
    int audioDemoCurrentButtonIndex;
} AudioDemo;



static void sampleFromDisk(AudioDemoButton *button, AudioDemo *demo) {
    print("DISK");
} // sampleFromDisk



// --------------------------------------------------

AudioDemoButton *buttonAtIndex(AudioDemo *demo, int index) {
    if (index >= demo->audioDemoButtonCount) {
        print("bad programmer, bad button index %d vs %d",
              index, demo->audioDemoButtonCount);
        return NULL;
    }

    return &demo->buttons[index];
} // buttonAtIndex


void drawGrid(AudioDemo *demo) {
    // screen is a grid of 16 buttons, 4x4

    const int width = kScreenWidth / kGridColumns;
    const int height = kScreenHeight / kGridRows;
    const int buttonMargin = 8;

    pd->graphics->setFont(buttonFont);

    for (int i = 0; i < demo->audioDemoButtonCount; i++) {
        AudioDemoButton *button = &demo->buttons[i];

        int row = i / kGridColumns;
        int column = (i % kGridColumns);

        Rect rect = (Rect){ width * column, height * row,
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


void moveButton(AudioDemo *demo, GridDirection direction) {
    int row = demo->audioDemoCurrentButtonIndex / kGridColumns;
    int column = demo->audioDemoCurrentButtonIndex % kGridColumns;
    
    switch (direction) {
    case kGridDirectionUp:
        row--;
        break;
    case kGridDirectionDown:
        row++;
        break;
    case kGridDirectionLeft:
        column--;
        break;
    case kGridDirectionRight:
        column++;
        break;
    }

    if (row < 0) row = kGridRows - 1;
    if (column < 0) column = kGridColumns - 1;
    if (row >= kGridRows) row = 0;
    if (column >= kGridColumns) column = 0;

    int index = row * kGridColumns + column;
    demo->audioDemoCurrentButtonIndex = index;

    demo->isDirty = true;

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
    demo->audioDemoButtonCount = kGridRows * kGridColumns;
    demo->audioDemoCurrentButtonIndex = 0;

    AudioDemoButton *sampleFromDiskButton = buttonAtIndex(demo, 0);
    sampleFromDiskButton->labelText = "Sample/File";
    sampleFromDiskButton->callback = sampleFromDisk;
    sampleFromDiskButton->demo = demo;

    for (int i = 1; i < kGridRows * kGridColumns; i++) {
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


