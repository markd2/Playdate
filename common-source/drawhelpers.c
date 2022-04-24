#include "drawhelpers.h"

#include "globals.h"

void strokeRect(Rect rect, LCDColor withColor) {
    pd->graphics->drawRect(rect.x, rect.y,
                           rect.width, rect.height,
                           withColor);
} // strokeRect
