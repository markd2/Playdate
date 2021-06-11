#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

#include "pd_api.h"

#define print pd->system->logToConsole 

void *pdMalloc(size_t size);
void *pdRealloc(void *memory, size_t size);
void pdFree(void *memory);
const char *nameForButton(PDButtons button);


static LCDFont *font;

static const int kScreenWidth = LCD_COLUMNS;
static const int kScreenHeight = LCD_ROWS;

static const int kTextWidth = 86;
static const int kTextHeight = 16;

// give this the raw value from getButtonState
typedef struct ButtonPumper ButtonPumper;

// pumper, pushed, released
typedef void ButtonPumperPumper(ButtonPumper*, PDButtons, PDButtons);

typedef enum {
    kPressed,
    kReleased
} UpDown;

// this get called when there's a change.
// called once for each button type when something changes
typedef void ButtonPumperCallback(PDButtons, UpDown);

struct ButtonPumper {
    PDButtons lastPushed;
    ButtonPumperPumper *pump;
    ButtonPumperCallback *callback;
};

void pumper(ButtonPumper *pumper, PDButtons pushed, PDButtons released) {
    PDButtons buttons[] = {
        kButtonLeft,
        kButtonRight,
        kButtonUp,
        kButtonDown,
        kButtonB,
        kButtonA
    };
    for (int i = 0; i < sizeof(buttons) / sizeof(*buttons); i++) {
        int mask = buttons[i];

        if (pushed & mask) {
            // if not pushed
            if (!(pumper->lastPushed & mask)) {
                // print("button down! %d", i);
                pumper->lastPushed |= mask;
                pumper->callback(mask, kPressed);
                continue;
            }
        }

        if (released & mask) {
            // if pushed
            if (pumper->lastPushed & mask) {
                // print("button up! %d", i);
                pumper->lastPushed &= ~mask;
                pumper->callback(mask, kReleased);
                continue;
            }
        }
    }
} // pumper


void callback(PDButtons button, UpDown state) {
    if (state == kReleased) {
        print("UP %s", nameForButton(button));
    }

    if (state == kPressed) {
        print("DOWN %s", nameForButton(button));
    }
} // callback

static ButtonPumper buttonPumper;

void checkButtons(void) {
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);

    buttonPumper.pump(&buttonPumper, pushed, released);

} // checkButtons


static int update(void *userdata) {

    checkButtons();

    // Move

    static int x = (kScreenWidth - kTextWidth) / 2;
    static int y = (kScreenHeight - kTextHeight) / 2;
    static int dx = 1;
    static int dy = 2;

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawText("Greeble Bork!", strlen("Greeble Bork!"), kASCIIEncoding, x, y);

    x += dx; y += dy;
	
    if (x < 0 || x > kScreenWidth - kTextWidth) {
        dx = -dx;
    }
	
    if (y < 0 || y > kScreenHeight - kTextHeight) {
        dy = -dy;
    }
        
    pd->system->drawFPS(0,0);

    return 1;
} // update


// --------------------------------------------------

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

const char *nameForButton(PDButtons button) {
    static PDButtons buttons[] = {
        kButtonLeft,
        kButtonRight,
        kButtonUp,
        kButtonDown,
        kButtonB,
        kButtonA
    };
    static const char *buttonNames[] = {
        "Left",
        "Right",
        "Up",
        "Down",
        "button B",
        "button A"
    };

    for (int i = 0; i < sizeof(buttons) / sizeof(*buttons); i++) {
        if (button == buttons[i]) {
            return buttonNames[i];
        }
    }

    return "---";
    
} // nameForButton


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event, 
                 uint32_t arg) {
    if (pd == NULL) {
        pd = playdate;
    }

    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit:
        pd->display->setRefreshRate(20);
        pd->system->setUpdateCallback(update, NULL);
        
        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        buttonPumper.pump = pumper;
        buttonPumper.callback = callback;
        break;

    case kEventInitLua:
        break;

    case kEventLock:
        break;

    case kEventUnlock:
        break;

    case kEventPause:
        break;

    case kEventResume:
        break;

    case kEventTerminate:
        break;

    case kEventKeyPressed: // arg is keycode
        break;

    case kEventKeyReleased:
        break;

    case kEventLowPower:
        break;
    }
    
    return 0;
} // eventHandler



// --------------------------------------------------
// Utilities

// wrappers for memory allocation that matches malloc/realloc/free

// Allocates heap space if ptr is NULL, 
// else reallocates the given pointer. 
// If size is zero, frees the given pointer.

void *pdMalloc(size_t size) {
    return pd->system->realloc(NULL, size);
} // pdMalloc


void *pdRealloc(void *memory, size_t size) {
    return pd->system->realloc(memory, size);
} // pdRealloc


void pdFree(void *memory) {
    pd->system->realloc(memory, 0);
} // pdFree

