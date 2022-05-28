#include "drawhelpers.h"

#include "globals.h"

void strokeRect(Rect rect, LCDColor withColor) {
    pd->graphics->drawRect(rect.x, rect.y,
                           rect.width, rect.height,
                           withColor);
} // strokeRect


void drawCString(const char *string, Point at) {
    pd->graphics->drawText(string, strlen(string),
                           kASCIIEncoding, at.x, at.y);
} // drawCString


void fillRect(Rect rect, LCDColor color) {
    pd->graphics->fillRect(rect.x, rect.y, rect.width, rect.height, color);
} // fillRect


void frameRect(Rect rect, LCDColor color) {
    pd->graphics->drawRect(rect.x, rect.y, rect.width, rect.height, color);
} // frameRect
