#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

static const int screenWidth = 400;
static const int screenHeight = 240;

static void drawStaticBackground(void);

static int update(void* userdata) {
    pd = userdata;
	
    // pd->graphics->clear(kColorWhite);
    pd->display->setRefreshRate(0);
        
    drawStaticBackground();

    pd->system->drawFPS(0, 0);

    return 1;
} // update


static void drawNoiseForRow(int y) {
    uint8_t *frameBuffer = pd->graphics->getFrame();
    uint8_t *scan;

    scan = frameBuffer + y * LCD_ROWSIZE;
    for (int x = 0; x < screenWidth / 8; x++) {
        char byte = rand() % 256;
        
        *scan++ = byte;
        // *scan++ = 0xF5; // clear bits are black, set bits are white  So ****o*o*. MSB first
    }

    pd->graphics->markUpdatedRows(y, y + 1);

} // drawNoiseForRow


static void drawStaticBackground(void) {
    int y = rand() % screenHeight;

    drawNoiseForRow(y);

    // whole screen
    // pd->graphics->markUpdatedRows(0, screenHeight - 1);

} // drawStaticBackground


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
