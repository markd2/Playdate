#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"
#include "memory.h"
#include "spy.h"

#include "pd_api.h"

const char *nameForButton(PDButtons button);


static LCDFont *font;

static const int kScreenWidth = LCD_COLUMNS;
static const int kScreenHeight = LCD_ROWS;

static const int kTextWidth = 86;
static const int kTextHeight = 16;

ButtonPumper *pumper;

static int scaleIndex = 0;
static const int scales[] = { 1, 2, 4, 8 };
static int inverted = 0;
static int flipped = 0;

void callback(PDButtons button, UpDown state) {
    print("-----");
    if (state == kReleased) {
        print("UP %s - time %d", nameForButton(button), pd->system->getCurrentTimeMilliseconds());
        unsigned int sec, ms;
        sec = pd->system->getSecondsSinceEpoch(&ms);
        print("  - time2 %d.%d", sec, ms);
    }

    if (state == kPressed) {
        print("DOWN %s", nameForButton(button));
        char *formatted;
        pd->system->formatString(&formatted, "hello %s", "sailor");
        print(formatted);
        pdFree(formatted);
    }

    if (button == kButtonUp) {
        print("scale %d", scales[scaleIndex]);  // no getScale
        print("height %d", pd->display->getHeight());
        print("width %d", pd->display->getWidth());
    }

    if (state == kPressed && button == kButtonDown) {
        scaleIndex = (scaleIndex + 1) % (sizeof(scales) / sizeof(*scales));

        pd->display->setScale(scales[scaleIndex]);
    }

    // left button to toggle invert
    if (state == kPressed && button == kButtonLeft) {
        inverted = !inverted;
        pd->display->setInverted(inverted);
    }

    // right button to cycle through flips
    if (state == kPressed && button == kButtonRight) {
        flipped = (flipped + 1) % 4;
        print("flippa da bits %d %d", (flipped & 10) >> 1, flipped & 01);
        pd->display->setFlipped((flipped & 10) >> 1, flipped & 01);
    }

    if (state == kPressed && button == kButtonUp) {
        int x = 2;
        int y = 3;
        print("MOSAIC %d %d", x, y);
        pd->display->setMosaic(x, y);
    }

} // callback


void checkButtons(void) {
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(pumper, pushed, released);

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


void menuItemCallback(void *userdata) {
    print("menu item callback: %p", userdata);
} // menuItemCallback


PDMenuItem *snorgle;
PDMenuItem *torgle;

static const char *options[] = {
    "Oøp",
    "Ack",
    "Greeble",
    "Bork"
};

void setupMenu(void) {
    
    pd->system->removeAllMenuItems();


    snorgle = pd->system->addMenuItem("Snorgle",
                                      menuItemCallback,
                                      (void *)"23");
#if 0
    PDMenuItem *menuItem;
//    menuItem = pd->system->addMenuItem("Greeble",
//                                       menuItemCallback,
//                                       (void *)"42");
//    menuItem = pd->system->addMenuItem("Bork Bork Bork Bork BORK",
//                                       menuItemCallback,
//                                       (void *)"11");
    torgle = pd->system->addCheckmarkMenuItem("Hoover", 1, menuItemCallback, "hello");
//    menuItem = pd->system->addCheckmarkMenuItem("Hoover", 0, menuItemCallback, "bork");

    menuItem = pd->system->addOptionsMenuItem("Blorf", options,
                                              sizeof(options) / sizeof(*options),
                                              menuItemCallback, "userdata?");
#endif

    const char *path = "images/menuImage";
    const char *error;

    LCDBitmap *bitmap = pd->graphics->loadBitmap(path, &error);

    if (error != NULL) {
        print("error is '%s'", error);
    } 

    if (bitmap != NULL) {
        pd->system->setMenuImage(bitmap, 20);
    }

} // setupMenu


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event, 
                 uint32_t arg) {
    if (pd == NULL) {
        pd = playdate;
        installSpies();
    }

    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit:
        pd->display->setRefreshRate(20);
        pd->system->setUpdateCallback(update, NULL);
        
        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        pumper = buttonPumperNew(callback);
        break;

    case kEventInitLua:
        break;

    case kEventLock:
        break;

    case kEventUnlock:
        break;

    case kEventPause:
        setupMenu();
        break;

    case kEventResume:
        print("hey!  back from menu!");
//        print("torgle value %d", pd->system->getMenuItemValue(torgle));
//        print("torgle userdata %s", pd->system->getMenuItemUserdata(torgle));
        print("snorgle userdata %s", pd->system->getMenuItemUserdata(snorgle));
        break;

    case kEventTerminate:
        uninstallSpies();
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
