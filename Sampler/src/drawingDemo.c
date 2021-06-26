#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

static const int kSlowSpeed = 1;
static const int kFastSpeed = 4;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Rect {
    int x;
    int y;
    int width;
    int height;
} Rect;

typedef struct DrawingDemo {
    DemoSample isa;
    ButtonPumper *pumper;
    int count;

    Rect ellipseRect;
    int ellipseAngle;
    int speed;
    
} DrawingDemo;


static void drawShapes(DrawingDemo *demo) {
    pd->graphics->clear(kColorWhite);
    
    int lineWidth = 2;
    float startAngle = demo->ellipseAngle;
    float endAngle = 0.0;
    LCDColor color = kColorBlack;
    pd->graphics->drawEllipse(demo->ellipseRect.x, demo->ellipseRect.y,
                              demo->ellipseRect.width, demo->ellipseRect.height,
                              lineWidth, startAngle, endAngle, color);
} // drawShapes


Rect clampToScreen(Rect rect) {
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;

    if (rect.x + rect.width > kScreenWidth) {
        rect.x = kScreenWidth - rect.width;
    }

    if (rect.y + rect.height > kScreenHeight) {
        rect.y = kScreenHeight - rect.height;
    }

    return rect;

} // clampToScreen


static void moveShapes(DrawingDemo *demo) {
    int speed = demo->speed;

    if (buttonPumperButtonIsDown(demo->pumper, kButtonLeft)) {
        demo->ellipseRect.x -= speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonRight)) {
        demo->ellipseRect.x += speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonUp)) {
        demo->ellipseRect.y -= speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonDown)) {
        demo->ellipseRect.y += speed;
    }

    demo->ellipseRect = clampToScreen(demo->ellipseRect);

} // moveShapes


static int update(void *context)  {
    DrawingDemo *demo = (DrawingDemo *)context;

    demo->count++;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    // dock crank to reset rotations.
    if (pd->system->isCrankDocked()) {
        demo->ellipseAngle = 0;
    } else {
        int crankAngle = pd->system->getCrankAngle();
        demo->ellipseAngle = crankAngle;
    }

    moveShapes(demo);
    drawShapes(demo);

    return 1;

} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    DrawingDemo *demo = (DrawingDemo *)context;

    if (buttons & kButtonA) {
        demo->speed = (upDown == kPressed) ? kFastSpeed : kSlowSpeed;
    }

} // handleButtons


DemoSample *drawingDemoSample(void) {
    DrawingDemo *demo = (DrawingDemo *)demoSampleNew("Drawing", kDrawing, 
                                                     update,
                                                     sizeof(DrawingDemo));
    demo->pumper = buttonPumperNew(handleButtons, demo);
    demo->count = 0;

    demo->ellipseRect = (Rect){ 0, 0, 50, 30 };
    demo->ellipseAngle = 0;
    demo->speed = 1;

    return (DemoSample *)demo;
} // drawingDemoSample


