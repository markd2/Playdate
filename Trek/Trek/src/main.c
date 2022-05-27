#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"


static ButtonPumper *pumper;
static LCDFont *appleFont;

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

static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (buttons == kButtonA && upDown == kPressed) {
        print("BUTTON A");
    }
} // handleButtons

static const int kVerticalDrawingOffset = kScreenHeight / 2 - 50;

static void draw(LCDFont *font, const char *string) {
    // kind of heavyweight
    pd->graphics->clear(kColorWhite);

    int textWidth = pd->graphics->getTextWidth(font,
                                               string, strlen(string),
                                               kASCIIEncoding, 0);
//    int centeringX = (kScreenWidth - textWidth) / 2.0;
    int centeringX = 2;
    int Y = 0;

    pd->graphics->setTextLeading(1);
    pd->graphics->setFont(font);
    pd->graphics->drawText(string, strlen(string), 
                           kASCIIEncoding, 
                           centeringX, Y);
} // draw


// --------------------------------------------------

static char *fullScreenString = ""
"   :   :   :   :   :   :   :   :   :   :\n"
"   :302:   :   :   :   :   :   :   :   :\n"
"   :   :   :   :   :   :   :   :   :   :\n"
"   :   :   :   :   :   :   :   :   :   :\n"
"   :   :   :   :   :   :   :   :   :   :\n"
"   :   :   :   :   :   :   :   :   :   :\n"
"   :   :   :   :   :   :   :   :   :   :\n"
"TRAJECTORY - 164\n"
"MISSED!!\n"
" <  KLINGON AT 4-3  FIRING PH TORP >\n"
"\n"
"\n"
" QUADRANT 3-1    SECTOR          3-4\n"
"              K1 YEARS           2.97\n"
"              K2 STARDTE         3424.0\n"
"     #E        3 CONDITION       RED\n"
"    K          4 SHIELDS         50%\n"
"              *5 SHIELD ENERGY   2189\n"
"               6 AVAIL ENERGY    2359\n"
"               7 PH TORPS        10\n"
"*              8 KLINGONS        41\n"
"1 2 3 4 5 6 7 8  BASES           4\n"
"                 COURSE NOT SET.";

static char *infoString = ""
" QUADRANT 3-1     SECTOR     3-4\n"
"              K1  YEARS      2.97\n"
"              K2  STARDTE    3424.0\n"
"     #E        3  CONDITION  RED\n"
"    K          4  SHIELDS    50%\n"
"              *5  SHIELD NRG 2189\n"
"               6  AVAIL NRG  2359\n"
"               7  PH TORPS   10\n"
"*              8  KLINGONS   41\n"
"1 2 3 4 5 6 7 8   BASES      4\n"
"                  COURSE NOT SET.";


static int flippa = 1;
static int updateDisplay(void) {
    if (flippa) {
        draw(appleFont, infoString);
        flippa = 0;
        return 1; // 1 to update the disply, 0 to naught do it.
    } else {
        return 0;
    }
} // updateDisplay



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
        pd->display->setRefreshRate(10);
        
        const char *errorText = NULL;
        appleFont = pd->graphics->loadFont("font/Apple2-14",
                                           &errorText);
        if (appleFont == NULL) {
            print("no Apple ][ font");
        }
        pd->system->setUpdateCallback(update, NULL);
        pumper = buttonPumperNew(handleButtons, NULL);

        pd->system->setCrankSoundsDisabled(1);  // turn off sound
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
