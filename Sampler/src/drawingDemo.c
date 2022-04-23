// Drawing demo, for "Geometry" functions
//
// Does this stuff:
//  - draws an ellipse. If cranking happens, draws arc
//  - draws a triangle that rotates with the crank, using manual trig to calculate points
//    - using bespoke geometry functions
//  - fills in a rectangle with various patterns, including one loaded from the filesytem
//  - loads a custom font (Cabel) from the app bundle

#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "patterns.h"
#include "geometry.h"

#include "pd_api.h"

static const int kSlowSpeed = 1;
static const int kFastSpeed = 4;

static const int kFilledRectangleHeight = 30;

static LCDFont *font;

static LCDPattern fillPattern = {
    // pattern
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,

    // mask
    0b11110000,
    0b11110000,
    0b11110000,
    0b11110000,
    0b00001111,
    0b00001111,
    0b00001111,
    0b00001111,
};

static LCDPattern basketWeavePattern;

static uintptr_t patterns[] = {
    (uintptr_t)fillPattern,
    (uintptr_t)basketWeavePattern,
    (uintptr_t)bubblePattern,
    (uintptr_t)failwafflePattern,
    (uintptr_t)rickrackPattern,
    (uintptr_t)percent50Pattern,
    (uintptr_t)wavyPattern,
    (uintptr_t)kColorBlack,
    (uintptr_t)kColorWhite
};
static int currentPatternIndex;

typedef struct DrawingDemo {
    DemoSample isa;
    ButtonPumper *pumper;
    int count;

    // Dpad moves. Button A faster.  Crank chooses arc to draw.
    Rect ellipseRect;
    int ellipseAngle;
    int speed;

    // Dpad moves center.  Button A faster.  Crank rotates triangle.
    Point triangleCenterPoint;
    int triangleSize;
    int triangleAngle;

    // Button B to change fills.
    Rect filledRect;
    LCDColor fillColor;
    
} DrawingDemo;


static void drawShapes(DrawingDemo *demo) {
    pd->graphics->clear(kColorWhite);

    int lineWidth = 2;
    float startAngle = demo->ellipseAngle;
    float endAngle = (pd->system->isCrankDocked()) ? 0 : ((int)startAngle + 180) % 360;

    LCDColor color = kColorBlack;
    pd->graphics->drawEllipse(demo->ellipseRect.x, demo->ellipseRect.y,
                              demo->ellipseRect.width, demo->ellipseRect.height,
                              lineWidth, startAngle, endAngle, color);

    Triangle triangle = triangleAt(demo->triangleCenterPoint, demo->triangleSize,
                                   demo->triangleAngle);

    Triangle clampedTriangle = clampTriangleToScreen(triangle);

    pd->graphics->fillTriangle(clampedTriangle.p1.x, clampedTriangle.p1.y,
                               clampedTriangle.p2.x, clampedTriangle.p2.y,
                               clampedTriangle.p3.x, clampedTriangle.p3.y,
                               color);

    pd->graphics->fillRect(demo->filledRect.x, demo->filledRect.y,
                           demo->filledRect.width, demo->filledRect.height,
                           demo->fillColor);

    pd->graphics->drawRect(demo->filledRect.x, demo->filledRect.y,
                           demo->filledRect.width, demo->filledRect.height,
                           kColorBlack);
} // drawShapes


static void moveShapes(DrawingDemo *demo) {
    int speed = demo->speed;

    if (buttonPumperButtonIsDown(demo->pumper, kButtonLeft)) {
        demo->ellipseRect.x -= speed;
        demo->triangleCenterPoint.y -= speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonRight)) {
        demo->ellipseRect.x += speed;
        demo->triangleCenterPoint.y += speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonUp)) {
        demo->ellipseRect.y -= speed;
        demo->triangleCenterPoint.x -= speed;
    }

    if (buttonPumperButtonIsDown(demo->pumper, kButtonDown)) {
        demo->ellipseRect.y += speed;
        demo->triangleCenterPoint.x += speed;
    }

    demo->ellipseRect = clampRectToScreen(demo->ellipseRect);
    demo->triangleCenterPoint = clampPointToScreen(demo->triangleCenterPoint);

    demo->triangleAngle = pd->system->getCrankAngle();

} // moveShapes


void drawInfoString(DrawingDemo *demo) {
    const int kLeading = 17;

    static char *texts[] = {
        "dpad: move  (a) speed up ",
        "(b) change pattern",
        "crank: angles"
    };
    static int textLen[3];
    int count = sizeof(texts) / sizeof(*texts);

    if (textLen[0] == 0) {
        for (int i = 0; i < count; i++) {
            textLen[i] = strlen(texts[i]);
        }
    }

    int y = 0;

    pd->graphics->setFont(font);
    for (int i = 0; i < count; i++) {
        pd->graphics->drawText(texts[i], textLen[i], kASCIIEncoding, 5, y);
        y += kLeading;
    }

} // drawInfoString


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

    drawInfoString(demo);

    return 1;

} // update


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    DrawingDemo *demo = (DrawingDemo *)context;

    if (buttons & kButtonA) {
        demo->speed = (upDown == kPressed) ? kFastSpeed : kSlowSpeed;
    }

    if (buttons & kButtonB && upDown == kPressed) {
        currentPatternIndex = (currentPatternIndex + 1) % (sizeof(patterns) / sizeof(*patterns));
        demo->fillColor = patterns[currentPatternIndex];
    }

} // handleButtons


static void loadPatterns(void) {
    SDFile *file = pd->file->open("patterns/basket-weave.pattern", kFileRead);
    if (file == NULL) {
        print("could not open - %s", pd->file->geterr());
        return;
    }

    pd->file->read(file, basketWeavePattern, sizeof(basketWeavePattern));

} // loadPatterns


DemoSample *drawingDemoSample(void) {
    DrawingDemo *demo = (DrawingDemo *)demoSampleNew("Drawing", kDrawing, 
                                                     update,
                                                     sizeof(DrawingDemo));
    const char *errorText = NULL;
    // font = pd->graphics->loadFont("font/font-Bitmore-Medieval-Outlined.pft", &errorText);
    font = pd->graphics->loadFont("font/Sasser-Small-Caps", &errorText);
    if (errorText != NULL) {
        print("couldn't make font %s", errorText);
    }

    demo->pumper = buttonPumperNew(handleButtons, demo);
    demo->count = 0;

    demo->ellipseRect = (Rect){ 10, 60, 80, 30 };
    demo->ellipseAngle = 0;

    demo->triangleCenterPoint = (Point){ kScreenWidth / 2, kScreenHeight / 2 };
    demo->triangleSize = 24;
    demo->triangleAngle = 0;

    demo->filledRect = (Rect){ 0, kScreenHeight - kFilledRectangleHeight,
        kScreenWidth, kFilledRectangleHeight};
    demo->fillColor = patterns[currentPatternIndex];

    loadPatterns();

    demo->speed = 1;

    return (DemoSample *)demo;
} // drawingDemoSample


