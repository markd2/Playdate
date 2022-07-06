#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "globals.h"
#include "pd_api.h"
}

PlaydateAPI *pd;

class Blah {
public:
    virtual void draw();
};

void Blah::draw() {
    pd->graphics->drawLine(0, 240, 400, 0, 3, kColorBlack);
}

class Blah2 {
public:
    virtual void draw();
};

void Blah2::draw() {
    pd->graphics->drawLine(0, 0, 400, 240, 3, kColorBlack);
    pd->graphics->drawLine(0, 240, 400, 0, 3, kColorBlack);
}

Blah *blah;

// Return 1 to update the display, 0 to not update it
static int update(void *userdata) {
    blah->draw();
    return 1;
} // update


extern "C"
int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    playdate->system->setUpdateCallback(update, NULL);
    playdate->system->logToConsole("SPLUNGE");
    pd = playdate;

    blah = new Blah;
#if 0
    // NOTHING GOES HERE
    pd = playdate;
//    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        pd->display->setRefreshRate(2);
        LCDFont *font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);

        pd->system->setUpdateCallback(update, NULL);

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
#endif    
    return 0;

} // eventHandler
