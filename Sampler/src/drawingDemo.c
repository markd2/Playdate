#include <stdio.h>
#include <stdlib.h>
#include <math.h> // sin

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

    Point triangleCenterPoint;
    int triangleSize;
    int triangleAngle;
    
} DrawingDemo;

typedef struct Triangle {
    Point p1;
    Point p2;
    Point p3;
} Triangle;



float degreesToRadians(float degrees) {
    float radians = (M_PI * degrees) / 180.0;
    return radians;
} // degreesToRadians


Point pointPlus(Point thing1, Point thing2) {
    Point point = (Point) { thing1.x + thing2.x, thing1.y + thing2.y };
    return point;
} // pointPlus


static Triangle triangleAt(Point centeredAt, int size, int rotation) {
    Triangle triangle;

    float angle;

    angle = degreesToRadians(rotation);
    print("angle 1 %f", angle);
    triangle.p1 = (Point) { cos(angle) * size, sin(angle) * size };

    angle = degreesToRadians(rotation + 120);
    print("angle 2 %f", angle);
    triangle.p2 = (Point) { cos(angle) * size, sin(angle) * size };

    angle = degreesToRadians(rotation + 240);
    print("angle 3 %f", angle);
    triangle.p3 = (Point) { cos(angle) * size, sin(angle) * size };

    triangle.p1 = pointPlus(triangle.p1, centeredAt);    
    triangle.p2 = pointPlus(triangle.p2, centeredAt);    
    triangle.p3 = pointPlus(triangle.p3, centeredAt);    

    return triangle;

} // triangleAt


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

    print("%f %f   %f %f   %f %f",
          triangle.p1.x, triangle.p1.y,
          triangle.p2.x, triangle.p2.y,
          triangle.p3.x, triangle.p3.y);
        
    pd->graphics->fillTriangle(triangle.p1.x, triangle.p1.y,
                               triangle.p2.x, triangle.p2.y,
                               triangle.p3.x, triangle.p3.y,
                               color);
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

    demo->triangleAngle = pd->system->getCrankAngle();

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

    demo->triangleCenterPoint = (Point){ kScreenWidth / 2, kScreenHeight / 2 };
    demo->triangleSize = 20;
    demo->triangleAngle = 0;

    demo->speed = 1;

    return (DemoSample *)demo;
} // drawingDemoSample


