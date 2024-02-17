#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

static const int screenWidth = 400;
static const int screenHeight = 240;

typedef struct Rect {
    int x, y;
    int width, height;
} Rect;

static void drawStaticBackground(void);
static void drawNoiseForRow(int y);
static void moveAndDrawRect(Rect from, Rect to);

static Rect sprite = { 0, 0, 2 /*bytes*/, 20 /*rows*/ };

static int frameCounter = 0;

static int update(void* userdata) {
    pd = userdata;
	
    // pd->graphics->clear(kColorWhite);
    pd->display->setRefreshRate(0);
        
    drawStaticBackground();

    Rect oldRect = sprite;
    sprite.x += 1;
    sprite.y += (frameCounter % 5 == 0) ? 1 : 0;

    if (sprite.y + sprite.height > screenHeight) { sprite.y = 0; }
    moveAndDrawRect(oldRect, sprite);

    pd->system->drawFPS(0, 0);

    frameCounter++;

    return 1;
} // update


static void moveAndDrawRect(Rect from, Rect to) {
    uint8_t *frameBuffer = pd->graphics->getFrame();

    // fill in where rect was
    for (int y = from.y; y < from.y + from.height; y++) {
        drawNoiseForRow(y);
    }

    // render rectangle, slowly
    
    for (int y = to.y; y < to.y + to.height; y++) {
        uint8_t *scan = frameBuffer + y * LCD_ROWSIZE;

        for (int x = to.x; x < to.x + to.width; x++) {
            *(scan + x) = 0xFF;
        }
    }

    pd->graphics->markUpdatedRows(from.y, from.y + from.height);
    pd->graphics->markUpdatedRows(to.y, to.y + to.height);
    
} // moveAndDrawRect


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
