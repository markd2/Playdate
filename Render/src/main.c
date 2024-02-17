#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

static const int screenWidth = 400;
static const int screenHeight = 240;

typedef enum Color {
    Color_Dark,
    Color_Light
} Color;

typedef struct Rect {
    int x, y;
    int width, height;
} Rect;

static void drawStaticBackground(void);
static void drawNoiseForRow(int y);
static void moveAndDrawRect(Rect from, Rect to);
static void plotPoint(uint8_t *buffer, int x, int y, Color color);
static void drawLine(uint8_t *buffer);
static void fillRect(uint8_t *buffer, Rect rect, Color color);
static void moveSprite(uint8_t *buffer);


static Rect sprite = { 0, 0, 20, 30 };

static int frameCounter = 0;

static int update(void* userdata) {
    pd = userdata;

    uint8_t *frameBuffer = pd->graphics->getFrame();
	
    // pd->graphics->clear(kColorWhite);
    pd->display->setRefreshRate(0);
        
//    drawStaticBackground();

    moveSprite(frameBuffer);

    pd->system->drawFPS(0, 0);

    frameCounter++;

    return 1;
} // update


static void moveSprite(uint8_t *buffer) {

    Rect oldRect = sprite;
    sprite.x += 2;
    sprite.y += 1;
    if (sprite.x + sprite.width >= screenWidth) {
        sprite.x = 0;
    }
    if (sprite.y + sprite.height >= screenHeight) {
        sprite.y = 0;
    }

    fillRect(buffer, oldRect, Color_Light);
    fillRect(buffer, sprite, Color_Dark);

} // moveSprite


static void drawLine(uint8_t *buffer) {
    for (int y = 0; y < 150; y++) {
        plotPoint(buffer, y, y, Color_Dark);
    }
} // drawLine


static void fillRect(uint8_t *buffer, Rect rect, Color color) {
    for (int y = rect.y; y < rect.y + rect.height; y++) {
        for (int x = rect.x; x < rect.x + rect.width; x++) {
            plotPoint(buffer, x, y, color);
        }
    }

    pd->graphics->markUpdatedRows(rect.y, rect.y + rect.height);

} // fillRect


static void plotPoint(uint8_t *buffer, int x, int y, Color color) {

    uint8_t *byteAddress = buffer + (y * LCD_ROWSIZE) + (x / 8);
    uint8_t byte = *byteAddress;
    uint8_t bit = 7 - x % 8;
    uint8_t mask = 1 << bit;

    if (color == Color_Dark) {
        // clear the bit
        byte &= ~mask;
    } else {
        // set the bit
        byte |= mask;
    }

    *byteAddress = byte;

} // plotPoint


static void moveAndDrawRect(Rect from, Rect to) {

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
