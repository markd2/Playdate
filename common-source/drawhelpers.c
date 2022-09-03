#include "drawhelpers.h"

#include <stdarg.h> // for va_args
#include <stdio.h>  // for vsnprintf

#include "globals.h"
#include "stb_ds.h"


void strokeRect(Rect rect, LCDColor withColor) {
    pd->graphics->drawRect(rect.x, rect.y,
                           rect.width, rect.height,
                           withColor);
} // strokeRect


void fillRect(Rect rect, LCDColor color) {
    pd->graphics->fillRect(rect.x, rect.y, rect.width, rect.height, color);
} // fillRect


void frameRect(Rect rect, LCDColor color) {
    pd->graphics->drawRect(rect.x, rect.y, rect.width, rect.height, color);
} // frameRect


void drawCString(const char *string, Point at) {
    pd->graphics->drawText(string, strlen(string),
                           kASCIIEncoding, at.x, at.y);
} // drawCString

void drawCStringf(Point at, char *buffer, size_t size, const char *format, ...) {
    va_list argList;
    va_start (argList, format);

    vsnprintf(buffer, size, format, argList);
    drawCString(buffer, at);
} // drawCStringf


void drawCStringCenteredInRect(const char *string, Rect rect, LCDFont *font) {
    pd->graphics->pushContext(NULL); {
        pd->graphics->setFont(font);
        int textWidth = pd->graphics->getTextWidth(NULL, string, strlen(string),
                                                   kASCIIEncoding, 0);
        int fontHeight = pd->graphics->getFontHeight(font);
        Rect textRect = (Rect){ 0, 0, textWidth, fontHeight };
        Rect centered = rectCenteredIn(rect, textRect);
        Point at = (Point){ centered.x, centered.y };
        
        drawCString(string, at);
    } pd->graphics->popContext();

} // drawCStringCenteredInRect


// TODO (7/5/22): fix \n\n - doesn't advance properly
void drawWrappedString(const char *string,
                       LCDFont *withFont, Rect inRect,
                       WordWidthHash **wordWidthHash,
                       int newlineLeading) {
    pd->graphics->setFont(withFont);

    int lineLength = 0;
    int x = inRect.x;
    int y = inRect.y;

    const char *wordStart = string;
    const char *scan = string;
    const char *stop = string + strlen(string);

    int fontHeight = pd->graphics->getFontHeight(withFont);

    int spaceWidth = pd->graphics->getTextWidth(withFont, " ", 1,
                                                kASCIIEncoding, 0);
    char buffer[100];

    while (scan <= stop) {
        if (*scan == ' ' || *scan == '\n' || scan == stop) {
            int wordLength = scan - wordStart;

            strncpy(buffer, wordStart, wordLength);
            buffer[wordLength] = '\000';

            int width = shget(*wordWidthHash, buffer);
            if (width == 0) {
                width = pd->graphics->getTextWidth(withFont,
                                                   wordStart,
                                                   wordLength,
                                                   kASCIIEncoding,
                                                   0);
                shput(*wordWidthHash, buffer, width);
                int spoonflongle = shget(*wordWidthHash, buffer);
            }

            // too long to fit? wrap.
            if (lineLength + width > inRect.width) {
                x = inRect.x;
                y += fontHeight;
                lineLength = 0;
            }

            // overflew the rectangle?  Done!
            if (y > inRect.y + inRect.height - fontHeight) {
                break;
            }

            // draw
            int width2 = pd->graphics->drawText(wordStart,
                                                scan - wordStart,
                                                kASCIIEncoding, x, y);
            lineLength += width;
            x += width;

            if (*scan == '\n') {
                x = inRect.x;
                y += fontHeight + newlineLeading;
                lineLength = 0;
            }

            wordStart = ++scan;

            x += spaceWidth;
            lineLength += spaceWidth;
        }

        scan++;
    }

} // drawWrappedString
