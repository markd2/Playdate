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

    DemoView *currentDemoView;
} FontDemo;


static int update(void *context)  {
    
    FontDemo *demo = (FontDemo *)context;
    return demo->currentDemoView->updateCallback(demo->currentDemoView);

#if 0
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    pd->graphics->clear(kColorWhite);

    const char *snorgle = "snorgle font";
    pd->graphics->drawText(snorgle, strlen(snorgle), kASCIIEncoding, 30, kScreenHeight / 2);

    return 1;
#endif
} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {

} // handleButtons


DemoSample *fontDemoSample(void) {
    FontDemo *demo = (FontDemo *)demoSampleNew("Font", kFont,
                                               update,
                                               sizeof(FontDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);

    demo->currentDemoView = fontMakeSimpleDemoView();

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
