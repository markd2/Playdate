#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for isspace

#include "buttonpumper.h"
#include "demoSample.h"
#include "drawhelpers.h"
#include "geometry.h"
#include "globals.h"
#include "memory.h"
#include "timing.h"

#include "pd_api.h"
#include "stb_ds.h"

typedef struct DemoView {
    const char *name;
    PDCallbackFunction *updateCallback;
    ButtonPumperCallback *buttonCallback;
} DemoView;

DemoView *fontMakeSimpleDemoView(void);
DemoView *fontMakeWrappedTextDemoView(void);
DemoView *fontMakeScrollingTextDemoView(void);

typedef struct { char *key; int value; } WordWidthHash;


typedef struct FontDemo {
    DemoSample isa;
    ButtonPumper *pumper;

    DemoView *demoViews[3];
    int currentDemoViewIndex;

} FontDemo;


static int update(void *context)  {
    FontDemo *fontDemo = (FontDemo *)context;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(fontDemo->pumper, pushed, released);
    
    DemoView *demoPane = fontDemo->demoViews[fontDemo->currentDemoViewIndex];
    return demoPane->updateCallback(demoPane);

} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    FontDemo *fd = (FontDemo *)context;

    int count = sizeof(fd->demoViews) / sizeof(*fd->demoViews);

    if (buttons == kButtonLeft && upDown == kPressed) {
        fd->currentDemoViewIndex = fd->currentDemoViewIndex - 1;
        if (fd->currentDemoViewIndex < 0) {
            fd->currentDemoViewIndex = count - 1;
        }
    } else if (buttons == kButtonRight && upDown == kPressed) {
        fd->currentDemoViewIndex = (fd->currentDemoViewIndex + 1) % count;
    } else {
        DemoView *currentDemoView = fd->demoViews[fd->currentDemoViewIndex];

        if (currentDemoView->buttonCallback) {
            currentDemoView->buttonCallback(buttons, upDown, currentDemoView);
        }
    }
} // handleButtons


DemoSample *fontDemoSample(void) {
    FontDemo *demo = (FontDemo *)demoSampleNew("Font", kFont,
                                               update,
                                               sizeof(FontDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    demo->currentDemoViewIndex = 2; // start with scrolling text demo

    demo->demoViews[0] = fontMakeSimpleDemoView();
    demo->demoViews[1] = fontMakeWrappedTextDemoView();
    demo->demoViews[2] = fontMakeScrollingTextDemoView();
    // if you add another to this, go visit the FontDemo struct

    return (DemoSample *)demo;
} // drawingDemoSample


// ==================================================

int genericCallback(void *userdata) {
    DemoView *demoView = (DemoView *)userdata;

    pd->graphics->clear(kColorWhite);

    const char *string = demoView->name;
    pd->graphics->drawText(string, strlen(string),
                           kASCIIEncoding, 100, 100);
    return 1; // update screen
} // genericCallback

// --------------------------------------------------

typedef struct SimpleDemoView {
    DemoView isa;
} SimpleDemoView;

DemoView *fontMakeSimpleDemoView(void) {
    static SimpleDemoView view;

    view.isa.name = "Simple Demo";
    view.isa.updateCallback = genericCallback;
    view.isa.buttonCallback = NULL;

    return (DemoView *)&view;
} // fontMakeSimpleDemoView

// --------------------------------------------------

static const int kMaxCrankMargin = 200;
typedef struct WrappedDemoView {
    DemoView isa;
    LCDFont *textFont;

    LCDFont *fonts[6];
    WordWidthHash *wordWidthHashes[6];

    int currentFontIndex;

    int crankMargin;

} WrappedDemoView;

const char *wrappedText = "Once upon a midnight dreary, while I pondered, weak and weary, Over many a quaint and curious volume of forgotten lore-\n  While I nodded, nearly napping, suddenly there came a tapping, As of some one gently rapping, rapping at my chamber door.\n\"'Tis some visitor,\" I muttered, \"tapping at my chamber door-\n      Only this and nothing more.\"";

// Performance is pretty adequate - couldn't see a reduction of
// FPS when wrapping double the pirsig string.
void drawWrappedString(const char *string,
                       LCDFont *withFont, Rect inRect,
                       WordWidthHash **wordWidthHash) {
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
                y += fontHeight;
                lineLength = 0;
            }

            wordStart = ++scan;

            x += spaceWidth;
            lineLength += spaceWidth;
        }

        scan++;
    }

} // drawWrappedString


static int wrappedDemoUpdate(void *context) {
    WrappedDemoView *view = (WrappedDemoView *)context;
    pd->graphics->clear(kColorWhite);

    Rect screen = screenRect();
    Rect innerFrame = insetRect(screen, 5, 5);
    Rect wrapFrame = insetRect(screen, 15, 15);

    float crankValue = pd->system->getCrankChange();

    if (crankValue > 7.0f) {
        view->crankMargin += 10;
    } else if (crankValue > 3.5f) {
        view->crankMargin += 5;
    } else if (crankValue > 0.0f) {
        view->crankMargin += 1;
    } else if (crankValue < -7.0f) {
        view->crankMargin -= 10;
    } else if (crankValue < -3.5f) {
        view->crankMargin -= 5;
    } else if (crankValue < 0.0f) {
        view->crankMargin -= 1;
    }
    view->crankMargin = MAX(view->crankMargin, 0);
    view->crankMargin = MIN(view->crankMargin, kMaxCrankMargin);

    strokeRect(innerFrame, kColorBlack);
    wrapFrame.width -= view->crankMargin;
    strokeRect(wrapFrame, kColorBlack);

    pd->graphics->setFont(view->textFont);
    Point titlePoint = { 30, 0 };
    drawCString("Wrapped Text", titlePoint);

    LCDFont *font = view->fonts[view->currentFontIndex];
    drawWrappedString(wrappedText, font, wrapFrame,
                      &view->wordWidthHashes[view->currentFontIndex]);
//    drawWrappedString(warAndPeace, font, wrapFrame,
//                      &view->wordWidthHashes[view->currentFontIndex]);

    pd->system->drawFPS(30, kScreenHeight - 20);

    return 1; // update screen
    
} // wrappedDemoUpdate


static void wrappedTextHandleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (upDown != kPressed) return;

    WrappedDemoView *view = (WrappedDemoView *)context;

    int count = sizeof(view->fonts) / sizeof(*view->fonts);

    if (buttons == kButtonUp) {
        view->currentFontIndex = (view->currentFontIndex + 1) % count;
        
    } else if (buttons == kButtonDown) {
        view->currentFontIndex = view->currentFontIndex - 1;
        if (view->currentFontIndex < 0) {
            view->currentFontIndex = count - 1;
        }
    }
} // wrappedTextHandleButtons


DemoView *fontMakeWrappedTextDemoView(void) {
    static WrappedDemoView view;

    view.isa.name = "Wrapped Demo";
    view.isa.updateCallback = wrappedDemoUpdate;

    LCDFont *font;

    char *fontpaths[] = {
        "font/Roobert-11-Mono-Condensed",
        "font/font-Bitmore-Medieval-Outlined",
        "font/Asheville-Rounded-24-px",
        "font/font-Cuberick-Bold-Halved",
        "font/font-Cuberick-Bold",
        "font/font-pixieval-large-black-bold"
    };

    int fontCount = sizeof(fontpaths) / sizeof(*fontpaths);
    for (int i = 0; i < fontCount; i++) {
        const char *errorText = NULL;

        char *fontpath = fontpaths[i];
        font = pd->graphics->loadFont(fontpath, &errorText);
        if (font == NULL) {
            print("could not load font %s - %s", fontpath, errorText);
        }
        view.fonts[i] = font;
    }

    view.textFont = view.fonts[0];

    view.currentFontIndex = 0;
    view.crankMargin = 0;

    view.isa.buttonCallback = wrappedTextHandleButtons;

    for (int i = 0; i < sizeof(view.wordWidthHashes) / sizeof(*view.wordWidthHashes); i++) {
        view.wordWidthHashes[i] = NULL;
        sh_new_arena(view.wordWidthHashes[i]);

        // Make sure an initial allocation for the hash table is made
        // before passing it around.
        shput(view.wordWidthHashes[i], "prime", 0);
    }

    
    return (DemoView *)&view;
} // fontMakeWrappedTextDemoView


// --------------------------------------------------

typedef struct ScrollingDemoView {
    DemoView isa;

    const char *warAndPeace;
    WordWidthHash *wordWidthHash;

    LCDFont *textFont;

} ScrollingDemoView;

typedef struct WordPosition {
    const char *wordStart;
    int length;
} WordPosition;

int wordPositionEnd = 0;
WordPosition wordPositions[570000]; // W&P has 565397 words via `wc`

int wordWidth(ScrollingDemoView *view, WordPosition pos) {
    char buffer[64]; // widest word in W&P is 31 characters
    strncpy(buffer, pos.wordStart, pos.length);
    buffer[pos.length] = '\000';

    int width = shget(view->wordWidthHash, buffer);
    if (width == 0) {
//        width = pd->graphics->getTextWidth(view->textFont,
//                                           pos.wordStart,
//                                           pos.length,
//                                           kASCIIEncoding, 0);
        shput(view->wordWidthHash, buffer, 5);
    }
    return width;
} // wordWidth


int breakWordsAndWarmSizeCache(ScrollingDemoView *view) {

    const char *string = view->warAndPeace;
    const char *word = string;

    while (true) {
        if (*word == 0) break;
        if (wordPositionEnd >= sizeof(wordPositions) / sizeof(*wordPositions)) {
            print("Blew past end of words at position %d",
                  word - string);
            return wordPositionEnd;
        }

        // suck up spaces
        while (*word != 0 && isspace((int)*word)) word++;

        wordPositions[wordPositionEnd].wordStart = word;
        while (*word != 0 && !isspace((int)*word)) word++;

        wordPositions[wordPositionEnd].length = word - wordPositions[wordPositionEnd].wordStart;

        char buffer[64]; // widest word in W&P is 31 characters
        strncpy(buffer, wordPositions[wordPositionEnd].wordStart,
                wordPositions[wordPositionEnd].length);
        buffer[wordPositions[wordPositionEnd].length] = '\000';
        
        if (shget(view->wordWidthHash, buffer) == 0) {
            shput(view->wordWidthHash, buffer, 1);
            //(void)wordWidth(view, wordPositions[wordPositionEnd]);
        }

        wordPositionEnd++;
    }

    return wordPositionEnd;
} // breakWordsAndWarmSizeCache


void measureTexts(ScrollingDemoView *view) {
    int blah;
    TIMING_START;
    blah = breakWordsAndWarmSizeCache(view);
    TIMING_END;

    print("found %d words, %d unique", blah, hmlen(view->wordWidthHash));

    /* timings
     * just scan through W&P
     *  - 0.175 (3x in a row)
     * counting words
     *  - 0.477 (3x in a row)
     *          (m1mini takes .0446, so only an order of magnitude faster?)
     * longest word is Hofs-kriegs-wurst-schnapps-Rath (31 characters)
     *
     * uncached text measurement of each word
     *  - 8.491 / 8.489 / 8.487
     * cached text measurement - 7.16? that doesn't sound right.
     * just hash table manipulations, is 6.42 seconds.  
     *  - that doesn't sound right.
     */

} // measureTexts


DemoView *fontMakeScrollingTextDemoView(void) {
    static ScrollingDemoView view;

    view.isa.name = "Scrolling Demo";
    view.isa.updateCallback = genericCallback;
    view.isa.buttonCallback = NULL;

    view.wordWidthHash = NULL;
    sh_new_arena(view.wordWidthHash);

    // Make sure an initial allocation for the hash table is made
    // before passing it around.
    shput(view.wordWidthHash, "prime", 0);

    const char *filename = "text/war-and-peace.txt";
//    const char *filename = "text/war-and-peace-short.txt";
    SDFile *file = pd->file->open(filename, kFileRead);

    FileStat stat;
    pd->file->stat(filename, &stat);

    char *buffer = pdMalloc(stat.size + 1);
    pd->file->read(file, buffer, stat.size);
    pd->file->close(file);
    buffer[stat.size] = '\000';
    
    view.warAndPeace = buffer;

    const char *errorText = NULL;
    const char *fontpath = "font/font-Cuberick-Bold";
    view.textFont = pd->graphics->loadFont(fontpath, &errorText);
    if (view.textFont == NULL) {
        print("could not load font %s - %s", fontpath, errorText);
    }

    measureTexts(&view);

    return (DemoView *)&view;
} // fontMakeScrollingTextDemoView

