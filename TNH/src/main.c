#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"


static LCDFont *clockFont;
static ButtonPumper *pumper;

typedef enum {
    kStateIdle,
    kStateTiming
} TNHState;

static TNHState currentState = kStateIdle;
static unsigned int startTime = 0;

static const char *eventNames[] = {
    "kEventInit",
    "kEventInitLua",
    "kEventLock",
    "kEventUnlock",
    "kEventPause",
    "kEventResume",
    "kEventTerminate",
    "kEventKeyPressed", // arg is keycode
    "kEventKeyReleased",
    "kEventLowPower"
};

static const int kVerticalDrawingOffset = kScreenHeight / 2 - 50;

static void draw(const char *string) {
    // kind of heavyweight
    pd->graphics->clear(kColorWhite);

    int textWidth = pd->graphics->getTextWidth(clockFont,
                                               string, strlen(string),
                                               kASCIIEncoding, 0);
    int centeringX = (kScreenWidth - textWidth) / 2.0;

    print("LUB %d", centeringX);

    pd->graphics->setFont(clockFont);
    pd->graphics->drawText(string, strlen(string), 
                           kASCIIEncoding, 
                           centeringX,
                           kVerticalDrawingOffset);
} // draw


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (buttons == kButtonA && upDown == kPressed) {
        print("BUTTON A - STARTING");
        
        if (currentState == kStateIdle) {
            currentState= kStateTiming;
            draw("timing");
            startTime = pd->system->getSecondsSinceEpoch(NULL);
            
        } else if (currentState == kStateTiming) {
            currentState = kStateIdle;
            draw("idle");
        }

        print("  current state %d", currentState);
    }
} // handleButtons


static int updateDisplay(void) {
    if (currentState == kStateTiming) {
        unsigned int now = pd->system->getSecondsSinceEpoch(NULL);
        unsigned int elapsedTime = now - startTime;

        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;

        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        print(buffer);
        draw(buffer);
    }
    return 1;
} // updateDisplay


// --------------------------------------------------

// Return 1 to update the display, 0 to not update it
static int update(void *userdata) {
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(pumper, pushed, released);
    return updateDisplay();
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    // NOTHING GOES HERE
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        pd->display->setRefreshRate(2);
        
        const char *errorText = NULL;
        clockFont = pd->graphics->loadFont("font/Mikodacs-Clock",
                                               &errorText);
        pd->graphics->setFont(clockFont);

        pd->system->setUpdateCallback(update, NULL);
        pumper = buttonPumperNew(handleButtons, NULL);

        break;
    }

    case kEventPause:
        break;

    case kEventResume:
        break;

    case kEventTerminate:
        break;

    default: break;
    }
    
    return 0;

} // eventHandler
