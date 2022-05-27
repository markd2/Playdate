#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"


static LCDFont *clockFont;
static LCDFont *textFont;
static ButtonPumper *pumper;

typedef enum {
    kStateIdle,
    kStateTiming
} TNHState;

static TNHState currentState = kStateIdle;
static unsigned int startTime = 0;

static const int kAlertThreshold = 30 * 60;  // 30 minutes
static FilePlayer *thresholdSound;

static const int kVerticalDrawingOffset = kScreenHeight / 2 - 50;

static void draw(LCDFont *font, const char *string) {
    // kind of heavyweight
    pd->graphics->clear(kColorWhite);

    int textWidth = pd->graphics->getTextWidth(font,
                                               string, strlen(string),
                                               kASCIIEncoding, 0);
    int centeringX = (kScreenWidth - textWidth) / 2.0;

    pd->graphics->setFont(font);
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
            startTime = pd->system->getSecondsSinceEpoch(NULL);
            
        } else if (currentState == kStateTiming) {
            currentState = kStateIdle;
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
        draw(clockFont, buffer);

        if (elapsedTime == kAlertThreshold) {
            pd->sound->fileplayer->play(thresholdSound, 1);
        }
    } else {
        draw(textFont, "Focus on Starting!");
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
        if (clockFont == NULL) {
            print("no clock font");
        }
        errorText = NULL;
        textFont = pd->graphics->loadFont("font/Roobert-24-Medium",
                                          &errorText);
        if (textFont == NULL) {
            print("no text font");
        }

        pd->system->setUpdateCallback(update, NULL);
        pumper = buttonPumperNew(handleButtons, NULL);

        thresholdSound = pd->sound->fileplayer->newPlayer();

        int exists = pd->sound->fileplayer->loadIntoPlayer(thresholdSound, "sound/coin");
        if (!exists) {
            print("  SNORGLE NO SOUND FILE FOUND");
        }

//        pd->system->setCrankSoundsDisabled(1);  // turn off sound
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
