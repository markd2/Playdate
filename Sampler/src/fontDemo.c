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
    
    int crankMargin;

    int currentTopLine;

} ScrollingDemoView;

const char *topLinePositions[200000];

void dumpTopLinePositions(void) {

    for (int i = 0; i < 20; i++) {
        print("[%d] %.10s", i, topLinePositions[i] ?: "NULL");
    }
} // dumpTopLinePositions


void drawWrappedStringFromTopLine(const char *string,
                                  LCDFont *withFont, Rect inRect,
                                  int currentTopLine,
                                  WordWidthHash **wordWidthHash) {
    int lineCount = 0;

    if (topLinePositions[currentTopLine] == NULL) {
        if (currentTopLine != 0) {
            if (topLinePositions[currentTopLine - 1] != NULL) {
                print("oops, problem - currentTopLine %d has an empty prior",
                      currentTopLine);
                return;
            }
        }
        topLinePositions[currentTopLine] = string;
    }

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
    char buffer[64]; // longest W&P word is 31 characters

    while (true) {
        if (*scan == '\000') break;

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
                
                lineCount++;
                topLinePositions[currentTopLine + lineCount] = wordStart;
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

                lineCount++;
                scan++;
                topLinePositions[currentTopLine + lineCount] = scan;
            }

            wordStart = ++scan;

            x += spaceWidth;
            lineLength += spaceWidth;
        }

        scan++;
    }
    
} // drawWrappedStringFromTopLine


int scrollingDemoViewCallback(void *userdata) {
    ScrollingDemoView *view = (ScrollingDemoView *)userdata;
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
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Scrolling Text (line %d)",
             view->currentTopLine);
    drawCString(buffer, titlePoint);

    LCDFont *font = view->textFont;
    const char *topLine = topLinePositions[view->currentTopLine] ?: view->warAndPeace;

    drawWrappedStringFromTopLine(topLine, font, wrapFrame,
                                 view->currentTopLine,
                                 &view->wordWidthHash);
    dumpTopLinePositions();    
    print("---------");

    pd->system->drawFPS(30, kScreenHeight - 20);

    return 1;

} // scrollingDemoViewCallback


static void scrollingDemoHandleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (upDown != kPressed) return;

    ScrollingDemoView *view = (ScrollingDemoView *)context;

    if (buttons == kButtonUp) {
        view->currentTopLine--;
        if (view->currentTopLine < 0) view->currentTopLine = 0;
    } else if (buttons == kButtonDown) {
        view->currentTopLine++;
    }
    print("%d", view->currentTopLine);
    
} // scrollingDemoHandleButtons


DemoView *fontMakeScrollingTextDemoView(void) {
    static ScrollingDemoView view = { 0 };

    view.isa.name = "Scrolling Demo";
    view.isa.updateCallback = scrollingDemoViewCallback;
    view.isa.buttonCallback = scrollingDemoHandleButtons;

    view.wordWidthHash = NULL;
    sh_new_arena(view.wordWidthHash);

    // Make sure an initial allocation for the hash table is made
    // before passing it around.
    shput(view.wordWidthHash, "prime", 0);

    const char *filename = "text/war-and-peace.txt";
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

    return (DemoView *)&view;
} // fontMakeScrollingTextDemoView

