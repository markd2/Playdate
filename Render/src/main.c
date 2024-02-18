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

// static const uint8_t texture[2] = { 0xB2, 0x2B }; // 10110010 00101011
static const uint8_t texture[2] = { 0x35, 0x92 }; // 00110101 10010010  // dark dark light light...

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

static const int spriteWidth = 10;
static const int spriteHeight = 7;

typedef struct Sprite {
    Rect rect;
    int deltaX;
    int deltaY;
} Sprite;

static Sprite sprite = { { 0, 0, spriteWidth, spriteHeight }, 1, 1};
static Sprite sprites[150];

static void drawStaticBackground(void);
static void drawNoiseForRow(int y);
static void moveAndDrawRect(Rect from, Rect to);
static void plotPoint(uint8_t *buffer, int x, int y, Color color);
static void drawLine(uint8_t *buffer);
static void fillRect(uint8_t *buffer, Rect rect, Color color);
static void moveSprite(uint8_t *buffer, Sprite *sprite);
static void moveSprites(uint8_t *buffer);
static void bulkBlortFromTo(const uint8_t *from, uint8_t *to);
void drawTextureAt(uint8_t *buffer, int x, int y);

static void firstTimeSetup(void) {
    Sprite *scan, *stop;
    scan = sprites;
    stop = scan + sizeof(sprites) / sizeof(*sprites);

    int hmod = screenWidth - spriteWidth;
    int vmod = screenHeight - spriteHeight;

    while (scan < stop) {
        scan->rect.x = rand() % hmod;
        scan->rect.y = rand() % vmod;
        scan->rect.width = 8 + rand() % spriteWidth;
        scan->rect.height = 6 + rand() % spriteHeight;
        scan->deltaX = 1;
        if (rand() % 3) { scan->deltaX = 2; }

        scan->deltaY = 1;
        if (rand() % 5) { scan->deltaY = 2; }

        scan++;
    }
} // firstTimeSetup

static int update(void* userdata) {
    pd = userdata;

    uint8_t *frameBuffer = pd->graphics->getFrame();

//    bulkBlortFromTo(stripeScreen, frameBuffer);
//    bulkBlortFromTo(darkScreen, frameBuffer);
//    bulkBlortFromTo(lightScreen, frameBuffer);
	
    pd->graphics->clear(kColorWhite);
        
//    drawStaticBackground();

    moveSprites(frameBuffer);

//    for (int i = 40; i < 80 + 40; i++) {
//        drawTextureAt(frameBuffer, i - 40, i);
//    }

    pd->system->drawFPS(0, 0);

    return 1;
} // update


static uint8_t topMaskLookup[] = {
    0b11111110,
    0b11111100,
    0b11111000,
    0b11110000,
    0b11100000,
    0b11000000,
    0b10000000,
    0b00000000
};

void drawTextureAt(uint8_t *buffer, int x, int y) {

    uint8_t *byteAddress = buffer + (y * LCD_ROWSIZE) + (x / 8);
    uint8_t byte1;
    uint8_t byte2;
    uint8_t bit = 7 - x % 8;

    if (bit == 7) {
        // no shift necessary.  Just blort the two bytes
        byte1 = texture[0];
        byte2 = texture[1];
        *byteAddress = byte1;
        *(byteAddress + 1) = byte2;
    } else {
        // basically put bit 7 of text1 at bit in the frame buffer.
        // There's four chunks
        //  * top bits of texture[0] go to bottom bits of frame buffer byte
        //  * bottom bits of texture[0] go to top bits of the nextByte
        //  * top bits of texture[1] go to the bottom bits of nextByte
        //  * bottom bits of texture[1] go to the top bites of nextNextByte
        // and essentially can repeat this for textures bigger than two, but for now
        // this will do
        // 
        // so, assuming a texture bit pattern of 
        //       7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
        //     | A B C D E F G H || I J K L M N O P |
        //
        // and a frame buffer bit pattern of 
        //       7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
        //     | a b c d e f g h || i j k l m n o p || q r s t u v w x ||
        //
        // and putting them together gives
        
        //     bit == 4
        //           byte0              byte1              byte2
        //     | a b c d e f g h || i j k l m n o p || q r s t u v w x ||
        //           | A B C D E    F G H|I J K L M    N O P |

        uint8_t byte0 = *byteAddress;
        uint8_t byte1 = 0; // going to be blown away
        uint8_t byte2 = *(byteAddress + 2);

        uint8_t topMask = topMaskLookup[bit];
        uint8_t bottomMask = ~topMask;

        // byte0 - blast bottom bits to zero, shift texture over, OR in the bits
        // say for bit == 4, top mask is 11100000 and bottom mask is 00011111
        byte0 &= topMask;
        byte0 |= (texture[0] >> (7 - bit)) & bottomMask;
        byteAddress[0] = byte0;

        // byte1 - bottom bits from texture 0 at the top, and top bits of text 1 at the bottom
        // assemble the top
        byte1 |= (texture[0] << (bit + 1)) & topMask;
        // assemble the bottom
        byte1 |= (texture[1] >> (7 - bit)) & bottomMask;
        byteAddress[1] = byte1;

        // byte2 - top bits from texture 1 at the bottom
        // blast the top bits to zero
        byte2 &= bottomMask;
        // shift over texture[1]
        byte2 |= (texture[1] << (bit + 1)) & topMask;
        byteAddress[2] = byte2;
    }

    pd->graphics->markUpdatedRows(y, y + 1);
    
} // drawTextureAt


// This slows everything down to 50fps on the sim, 52fps on the device
static void bulkBlortFromTo(const uint8_t *from, uint8_t *to) {
    memcpy(to, from, screenBufferSize);
    // looked at https://cboard.cprogramming.com/c-programming/154333-fast-memcpy-alternative-32-bit-embedded-processor-posted-just-fyi-fwiw.html, but it still was 52 fps on the device.x
    pd->graphics->markUpdatedRows(0, screenHeight);

} // bulkBlort


static void moveSprites(uint8_t *buffer) {
    Sprite *scan, *stop;
    scan = sprites;
    stop = scan + sizeof(sprites) / sizeof(*sprites);

    while (scan < stop) {
        moveSprite(buffer, scan);
        scan++;
    }

} // moveSprites


static void moveSprite(uint8_t *buffer, Sprite *sprite) {

    Rect oldRect = sprite->rect;
    Rect newRect = sprite->rect;

    newRect.x += sprite->deltaX;
    newRect.y += sprite->deltaY;
    if (newRect.x + newRect.width >= screenWidth) {
        newRect.x = 0;
    }
    if (newRect.y + newRect.height >= screenHeight) {
        newRect.y = 0;
    }

//    fillRect(buffer, oldRect, Color_Light);
//    fillRect(buffer, newRect, Color_Dark);

    for (int y = newRect.y; y < newRect.y + newRect.height; y++) {
        drawTextureAt(buffer, newRect.x, y);
    }

    sprite->rect = newRect;

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
