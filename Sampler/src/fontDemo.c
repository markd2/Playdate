#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

typedef struct DemoView {
    const char *name;
    PDCallbackFunction *updateCallback;
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
    }
} // handleButtons


DemoSample *fontDemoSample(void) {
    FontDemo *demo = (FontDemo *)demoSampleNew("Font", kFont,
                                               update,
                                               sizeof(FontDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    demo->currentDemoViewIndex = 0;

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

    return (DemoView *)&view;
} // fontMakeSimpleDemoView

// --------------------------------------------------

typedef struct WrappedDemoView {
    DemoView isa;
} WrappedDemoView;

DemoView *fontMakeWrappedTextDemoView(void) {
    static WrappedDemoView view;

    view.isa.name = "Wrapped Demo";
    view.isa.updateCallback = genericCallback;

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

    return (DemoView *)&view;
} // fontMakeScrollingTextDemoView
