#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static int update(void* userdata) {
    PlaydateAPI *pd = userdata;
	
    pd->graphics->clear(kColorWhite);
        
    pd->system->drawFPS(0,0);

    return 1;
} // update


int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
    (void)arg; // arg is currently only used for event = kEventKeyPressed

    if (event == kEventInit ) {
        // If you set an update callback in the kEventInit
        // handler, the system assumes the game is pure C and doesn't
        // run any Lua code in the game
        pd->system->setUpdateCallback(update, pd);
    }
	
    return 0;
} // eventHandler
