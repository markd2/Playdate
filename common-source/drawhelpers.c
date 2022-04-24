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

