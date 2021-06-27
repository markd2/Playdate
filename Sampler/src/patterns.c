#include "patterns.h"

// white == 1

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
