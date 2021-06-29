#include "patterns.h"

// white == 1
// from MacPaint, black pixels are 0, white are 1

#define SOLID_MASK 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF


LCDPattern bubblePattern = {
    0b10001000,
    0b01110110,
    0b01110000,
    0b01110000,
    0b10001000,
    0b01100111,
    0b00000111,
    0b00000111,

    SOLID_MASK
};


LCDPattern failwafflePattern = {
    0b01000000,
    0b11111111,
    0b01000000,
    0b01000000,
    0b01001111,
    0b01001111,
    0b01001111,
    0b01001111,

    SOLID_MASK
};


LCDPattern rickrackPattern = {
    0b01110111,
    0b11101011,
    0b11011101,
    0b10111110,
    0b01110111,
    0b11111111,
    0b01010101,
    0b11111111,

    SOLID_MASK
};


LCDPattern percent50Pattern = {
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,

    SOLID_MASK
};

LCDPattern wavyPattern = {
    0b11011111,
    0b10101111,
    0b01110111,
    0b01110111,
    0b01110111,
    0b01110111,
    0b11111010,
    0b11111101,

    SOLID_MASK
};

LCDPattern basketWeavePatternOpen = {
    0b11111000,
    0b01110100,
    0b00100010,
    0b01000111,
    0b10001111,
    0b00010111,
    0b00100010,
    0b01110001,
    
    0b00000111,
    0b10001011,
    0b11011101,
    0b10111000,
    0b01110000,
    0b11101000,
    0b11011101,
    0b10001110,
};
