#ifndef BORK_DRAWHELPERS_H
#define BORK_DRAWHELPERS_H

#include <stdlib.h>

#ifndef PLAYDATEAPI_H
#include "pd_api.h"
#endif

#include "geometry.h"

void frameRect(Rect rect, LCDColor color);
void fillRect(Rect rect, LCDColor color);

// need a takes-font and clear-background-to-color version too.
void drawCString(const char *string, Point at);
void drawCStringCenteredInRect(const char *string, Rect rect, LCDFont *font);

typedef struct { char *key; int value; } WordWidthHash;
void drawWrappedString(const char *string,
                       LCDFont *withFont, Rect inRect,
                       WordWidthHash **wordWidthHash,
                       int newlineLeading);


#endif // BORK_DRAWHELPERS_H
