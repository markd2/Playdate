#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

static const int screenWidth = 400;
static const int screenHeight = 240;

static void draw(void);

static int update(void* userdata) {
    pd = userdata;
	
    pd->graphics->clear(kColorWhite);
    pd->display->setRefreshRate(50);
        
    draw();

    pd->system->drawFPS(0,0);

    return 1;
} // update


static void draw(void) {
    uint8_t *frameBuffer = pd->graphics->getFrame();
    uint8_t *scan;

    for (int y = 0; y < screenHeight; y += 1) {
        scan = frameBuffer + y * LCD_ROWSIZE;
        for (int x = 0; x < screenWidth / 8; x++) {
            *scan++ = 0xF5; // clear bits are black, set bits are white
        }
    }
} // draw


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
