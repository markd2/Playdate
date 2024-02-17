#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

static const int screenWidth = 400;
static const int screenHeight = 240;

static void draw(char bit0, char bit1, char bit2, char bit3, char bit4,char bit5, char bit6, char bit7);

static int update(void* userdata) {
    pd = userdata;
	
    pd->graphics->clear(kColorWhite);
    pd->display->setRefreshRate(50);
    
    
        
    draw(rand() %2, rand() %2, rand() %2, rand() %2, rand() %2, rand() %2, rand() %2, rand() %2);

    pd->system->drawFPS(0,0);

    return 1;
} // update


static void draw(char bit0, char bit1, char bit2, char bit3, char bit4,char bit5, char bit6, char bit7) {
    uint8_t *frameBuffer = pd->graphics->getFrame();
    uint8_t *scan;

    for (int y = 0; y < screenHeight; y += 1) {
        scan = frameBuffer + y * LCD_ROWSIZE;
        for (int x = 0; x < screenWidth / 8; x++) {
            char byte = 0;
            if (bit0) {
                byte |= 1 << 0;
            }
            if (bit1) {
                byte |= 1 << 1;
            }
            if (bit2) {
                byte |= 1 << 2;
            }
            if (bit3) {
                byte |= 1 << 3;
            }
            if (bit4) {
                byte |= 1 << 4;
            }
            if (bit5) {
                byte |= 1 << 5;
            }
            if (bit6) {
                byte |= 1 << 6;
            }
            if (bit7) {
                byte |= 1 << 7;
            }

            *scan++ = byte;
            // *scan++ = 0xF5; // clear bits are black, set bits are white
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
