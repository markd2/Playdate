#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "drawhelpers.h"
#include "geometry.h"
#include "globals.h"

#include "pd_api.h"

typedef struct DemoView {
    const char *name;
    PDCallbackFunction *updateCallback;
    ButtonPumperCallback *buttonCallback;
} DemoView;

DemoView *fontMakeSimpleDemoView(void);
DemoView *fontMakeWrappedTextDemoView(void);
DemoView *fontMakeScrollingTextDemoView(void);


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

    demo->currentDemoViewIndex = 1; // start with wrapped demo

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

typedef struct WrappedDemoView {
    DemoView isa;
    LCDFont *textFont;

    LCDFont *fonts[6];
    int currentFontIndex;
} WrappedDemoView;

const char *wrappedText = "Metaphysics is a restaurant where they give you a thirty-thousand-page menu and no food.\n-- Robert M. Pirsig";

void drawWrappedString(const char *string,
                       LCDFont *withFont, Rect inRect) {
    pd->graphics->setFont(withFont);

    int lineLength = 0;
    int x = inRect.x;
    int y = inRect.y;

    const char *wordStart = string;
    const char *scan = string;
    const char *stop = string + strlen(string);

    while (scan < stop) {
        if (*scan == ' ') {
            int width = pd->graphics->drawText(wordStart,
                                               scan - wordStart,
                                               kASCIIEncoding, x, y);

            lineLength += width;
            x += width;
            wordStart = scan++;
        }

        if (lineLength > inRect.width) {
            x = inRect.x;
            y += pd->graphics->getFontHeight(withFont);
            lineLength = 0;
        }
        
        scan++;
    }


//    pd->graphics->drawText(string, strlen(string),
//                           kASCIIEncoding, inRect.x, inRect.y);
} // drawWrappedString


static int wrappedDemoUpdate(void *context) {
    WrappedDemoView *view = (WrappedDemoView *)context;
    pd->graphics->clear(kColorWhite);

    Rect screen = screenRect();
    Rect innerFrame = insetRect(screen, 5, 5);
    Rect wrapFrame = insetRect(screen, 15, 15);

    strokeRect(innerFrame, kColorBlack);
    strokeRect(wrapFrame, kColorBlack);

    pd->graphics->setFont(view->textFont);
    Point titlePoint = { 30, 0 };
    drawCString("Wrapped Text", titlePoint);

    LCDFont *font = view->fonts[view->currentFontIndex];
    drawWrappedString(wrappedText, font, wrapFrame);

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

    view.isa.buttonCallback = wrappedTextHandleButtons;

    return (DemoView *)&view;
} // fontMakeWrappedTextDemoView


// --------------------------------------------------

typedef struct ScrollingDemoView {
    DemoView isa;
} ScrollingDemoView;

DemoView *fontMakeScrollingTextDemoView(void) {
    static ScrollingDemoView view;

    view.isa.name = "Scrolling Demo";
    view.isa.updateCallback = genericCallback;
    view.isa.buttonCallback = NULL;

    return (DemoView *)&view;
} // fontMakeScrollingTextDemoView
