#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    bool isDirty;

    LCDBitmap *menuImageBitmap;
    LCDFont *menuImageFont;
    WordWidthHash *menuImageWordWidthHash;
    const char *menuText;
} DemoView;

DemoView *fontMakeMarkdownDemoView(void);
DemoView *fontMakeWrappedTextDemoView(void);
DemoView *fontMakeScrollingTextDemoView(void);

static void commonInit(DemoView *demoView, const char *menuText) {
    bzero(demoView, sizeof(*demoView));

    const char *errorText = NULL;
    const char *fontpath = "font/Roobert-11-Mono-Condensed";
    demoView->menuImageFont = pd->graphics->loadFont(fontpath, &errorText);
    if (demoView->menuImageFont == NULL) {
        print("could not load font %s - %s", fontpath, errorText);
    }
    sh_new_arena(demoView->menuImageWordWidthHash);

    // Make sure an initial allocation for the hash table is made
    // before passing it around.
    shput(demoView->menuImageWordWidthHash, "prime", 0);

    demoView->menuText = menuText;

} // commonInit


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
        fd->demoViews[fd->currentDemoViewIndex]->isDirty = true;
    } else if (buttons == kButtonRight && upDown == kPressed) {
        fd->currentDemoViewIndex = (fd->currentDemoViewIndex + 1) % count;
        fd->demoViews[fd->currentDemoViewIndex]->isDirty = true;
    } else {
        DemoView *currentDemoView = fd->demoViews[fd->currentDemoViewIndex];

        if (currentDemoView->buttonCallback) {
            currentDemoView->buttonCallback(buttons, upDown, currentDemoView);
        }
    }
    print("CURRENT INDEX IS %d", fd->currentDemoViewIndex);
} // handleButtons



static LCDBitmap *menuImageCallback(DemoSample *sample, int *outXOffset) {
    FontDemo *fontDemo = (FontDemo *)sample;

    DemoView *demoView = fontDemo->demoViews[fontDemo->currentDemoViewIndex];

    if (demoView->menuImageBitmap == NULL) {
        demoView->menuImageBitmap = pd->graphics->newBitmap(kScreenWidth, kScreenHeight, kColorWhite);
        Rect rect = (Rect){ kScreenWidth / 2, 0, 
                            kScreenWidth / 2, kScreenHeight };
        Rect insetRect = rectInset(rect, 10, 10);
        pd->graphics->pushContext(demoView->menuImageBitmap); {
            demoView->isDirty = true;
            demoView->updateCallback(demoView);

            fillRect(rect, kColorWhite);
            drawWrappedString(demoView->menuText,
                              demoView->menuImageFont, insetRect,
                              &demoView->menuImageWordWidthHash,
                              6);

        } pd->graphics->popContext();
    }

    if (outXOffset != NULL) {
        *outXOffset = kScreenWidth / 2;
    }

    return demoView->menuImageBitmap;

} // menuImageCallback


DemoSample *fontDemoSample(void) {
    FontDemo *demo = (FontDemo *)demoSampleNew("Text", kFont,
                                               update,
                                               sizeof(FontDemo));
    demo->isa.menuImageCallback = menuImageCallback;
    demo->pumper = buttonPumperNew(handleButtons, demo);

    demo->currentDemoViewIndex = 2; // start with scrolling text demo

    demo->demoViews[0] = fontMakeMarkdownDemoView();
    demo->demoViews[1] = fontMakeWrappedTextDemoView();
    demo->demoViews[2] = fontMakeScrollingTextDemoView();
    // if you add another to this, go visit the FontDemo struct

    return (DemoSample *)demo;
} // drawingDemoSample


// ==================================================

int markdownCallback(void *userdata) {
    DemoView *demoView = (DemoView *)userdata;

    pd->graphics->clear(kColorWhite);

    const char *string = demoView->name;
    pd->graphics->drawText(string, strlen(string),
                           kASCIIEncoding, 100, 100);
    return 1; // update screen
} // markdownCallback

// --------------------------------------------------

// asdf

/* Markdown Demo View - have some text we can scroll through, and create it
 * via markdown.
 *
 * [ ] have a source Markdown document
 * [ ] have an offline process to convert the Markdown to an easily loaded and
 *     parsable format
 * [ ] Make the above EL&PF (essentially an attributed string)
 * [ ] Draw said attributed string
 * [ ] Smoothly scroll
 * [ ] stretch goal - do the playback character by character
 */

typedef enum AttributedStringStyle {
    kStyleHeadline,
    kStylePlain,
    kStyleBold,
    kStyleItalic
} AttributedStringStyle;

typedef struct Attribute {
    int rangeStart;
    int rangeEnd;  // inclusive
    AttributedStringStyle style;
} Attribute;

typedef struct AttributedString {
    const char *string;
    Attribute attributes[100];
} AttributedString;

typedef struct MarkdownDemoView {
    DemoView isa;
    AttributedString *attributedString;
} MarkdownDemoView;

DemoView *fontMakeMarkdownDemoView(void) {
    static MarkdownDemoView view;
    const char *menuText = "MARKDOWN DEMO\n"
        "* So far just optimistic comments\n"
        "* Left/Right to change Text Tabs";

    commonInit(&view.isa, menuText);

    view.isa.name = "Markdown Demo";
    view.isa.updateCallback = markdownCallback;
    view.isa.buttonCallback = NULL;

    view.attributedString = pdMalloc(sizeof(view.attributedString));

    return (DemoView *)&view;
} // fontMakeMarkdownDemoView


// asdf


// ==================================================

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

static int wrappedDemoUpdate(void *context) {
    WrappedDemoView *view = (WrappedDemoView *)context;
    pd->graphics->clear(kColorWhite);

    Rect screen = screenRect();
    Rect innerFrame = rectInset(screen, 5, 5);
    Rect wrapFrame = rectInset(screen, 15, 15);

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

    frameRect(innerFrame, kColorBlack);
    wrapFrame.width -= view->crankMargin;
    frameRect(wrapFrame, kColorBlack);

    pd->graphics->setFont(view->textFont);
    Point titlePoint = { 30, 0 };
    drawCString("Wrapped Text", titlePoint);

    LCDFont *font = view->fonts[view->currentFontIndex];
    drawWrappedString(wrappedText, font, wrapFrame,
                      &view->wordWidthHashes[view->currentFontIndex], 0);
//    drawWrappedString(warAndPeace, font, wrapFrame,
//                      &view->wordWidthHashes[view->currentFontIndex], 0);

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
    const char *menuText = "WRAPPING DEMO\n"
        "* Up/Down to change font\n"
        "* Crank to change wrapping rectangle\n"
        "* Left/Right to change Text Tabs";

    commonInit(&view.isa, menuText);

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


int scrollingDemoViewUpdate(void *userdata) {
    ScrollingDemoView *view = (ScrollingDemoView *)userdata;

    if (!view->isa.isDirty) {
        pd->system->drawFPS(30, kScreenHeight - 20);
    }

    Rect screen = screenRect();
    Rect innerFrame = rectInset(screen, 5, 5);
    Rect wrapFrame = rectInset(screen, 15, 15);

    float crankValue = pd->system->getCrankChange();

    if (crankValue != 0.0f) {
        print("crank value %f", (double)crankValue);
    }
    if (crankValue > 7.0f) {
        view->currentTopLine += 11;
        view->isa.isDirty = true;
    } else if (crankValue > 3.5f) {
        view->currentTopLine += 5;
        view->isa.isDirty = true;
    } else if (crankValue > 0.0f) {
        view->currentTopLine += 1;
        view->isa.isDirty = true;
    } else if (crankValue < -7.0f) {
        view->currentTopLine -= 11;
        view->isa.isDirty = true;
    } else if (crankValue < -3.5f) {
        view->currentTopLine -= 5;
        view->isa.isDirty = true;
    } else if (crankValue < 0.0f) {
        view->currentTopLine -= 1;
        view->isa.isDirty = true;
    }
    view->currentTopLine = MAX(view->currentTopLine, 0);

    if (!view->isa.isDirty) return 1;
    view->isa.isDirty = false;

    pd->graphics->clear(kColorWhite);

    frameRect(innerFrame, kColorBlack);
    frameRect(wrapFrame, kColorBlack);

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
//    dumpTopLinePositions();    
//    print("---------");

    pd->system->drawFPS(30, kScreenHeight - 20);

    return 1;

} // scrollingDemoViewUpdate


static void scrollingDemoHandleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (upDown != kPressed) return;

    ScrollingDemoView *view = (ScrollingDemoView *)context;

    if (buttons == kButtonUp) {
        view->currentTopLine--;
        if (view->currentTopLine < 0) view->currentTopLine = 0;

        view->isa.isDirty = true;
    } else if (buttons == kButtonDown) {
        view->currentTopLine++;
        view->isa.isDirty = true;
    }
    print("%d", view->currentTopLine);
    
} // scrollingDemoHandleButtons


DemoView *fontMakeScrollingTextDemoView(void) {
    static ScrollingDemoView view;
    const char *menuText = "SCROLLING DEMO\n"
        "* Crank to scroll War and Peace\n"
        "* Up/Down to scroll by line\n"
        "* Left/Right to change Text Tabs";
    commonInit(&view.isa, menuText);

    view.isa.name = "Scrolling Demo";
    view.isa.updateCallback = scrollingDemoViewUpdate;
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

    view.isa.isDirty = true;

    return (DemoView *)&view;
} // fontMakeScrollingTextDemoView

