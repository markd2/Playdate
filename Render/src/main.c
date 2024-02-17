#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI *pd;

typedef enum Color {
    Color_Dark,
    Color_Light
} Color;

static const int screenWidth = 400;
static const int screenHeight = 240;

// use #define so we can statically "allocate" these buffer
#define screenBufferSize 12480 // screenHeight * LCD_ROWSIZE

//static const int screenBufferSize = 12480; // screenHeight * LCD_ROWSIZE;

static const uint8_t darkScreen[screenBufferSize] = { [0 ... screenBufferSize - 1] = Color_Dark };
static const uint8_t lightScreen[screenBufferSize] = { [0 ... screenBufferSize - 1] = Color_Light };
static const uint8_t stripeScreen[screenBufferSize] = { [0 ... screenBufferSize - 1] = 0xCC };

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
static void moveSprite(uint8_t *buffer, Rect *inoutSprite);
static void moveSprites(uint8_t *buffer);
static void bulkBlortFromTo(const uint8_t *from, uint8_t *to);

static const int spriteWidth = 10;
static const int spriteHeight = 7;

static Rect sprite = { 0, 0, spriteWidth, spriteHeight };
static Rect sprites[150];

static int frameCounter = 0;

static void firstTimeSetup(void) {
    Rect *scan, *stop;
    scan = sprites;
    stop = scan + sizeof(sprites) / sizeof(*sprites);

    int hmod = screenWidth - spriteWidth;
    int vmod = screenHeight - spriteHeight;

    while (scan < stop) {
        scan->x = rand() % hmod;
        scan->y = rand() % vmod;
        scan->width = 8 + rand() % spriteWidth;
        scan->height = 6 + rand() % spriteHeight;
        scan++;
    }
} // firstTimeSetup

static int update(void* userdata) {
    pd = userdata;

    uint8_t *frameBuffer = pd->graphics->getFrame();

//    bulkBlortFromTo(stripeScreen, frameBuffer);
//    bulkBlortFromTo(darkScreen, frameBuffer);
//    bulkBlortFromTo(lightScreen, frameBuffer);
	
//    pd->graphics->clear(kColorWhite);
        
//    drawStaticBackground();

    moveSprites(frameBuffer);

    pd->system->drawFPS(0, 0);

    frameCounter++;

    return 1;
} // update


// This slows everything down to 50fps on the sim, 52fps on the device
static void bulkBlortFromTo(const uint8_t *from, uint8_t *to) {
    memcpy(to, from, screenBufferSize);
    // looked at https://cboard.cprogramming.com/c-programming/154333-fast-memcpy-alternative-32-bit-embedded-processor-posted-just-fyi-fwiw.html, but it still was 52 fps on the device.x
    pd->graphics->markUpdatedRows(0, screenHeight);

} // bulkBlort


static void moveSprites(uint8_t *buffer) {
    Rect *scan, *stop;
    scan = sprites;
    stop = scan + sizeof(sprites) / sizeof(*sprites);

    while (scan < stop) {
        moveSprite(buffer, scan);
        scan++;
    }

} // moveSprites


static void moveSprite(uint8_t *buffer, Rect *inoutSprite) {

    Rect oldRect = *inoutSprite;
    Rect sprite = *inoutSprite;
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

    *inoutSprite = sprite;

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
        pd->display->setRefreshRate(0);

        firstTimeSetup();
    }
	
    return 0;
} // eventHandler
