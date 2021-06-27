#include <stdio.h>
#include <stdlib.h>
#include <math.h> // sin
#include <limits.h> // INT_MAX

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


Rect clampRectToScreen(Rect rect) {
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;

    if (rect.x + rect.width > kScreenWidth) {
        rect.x = kScreenWidth - rect.width;
    }

    if (rect.y + rect.height > kScreenHeight) {
        rect.y = kScreenHeight - rect.height;
    }

    return rect;

} // clampRectToScreen

#if !defined(MAX)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#if !defined(MIN)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif


Point clampPointToScreen(Point point) {
    int x = MAX(point.x, 0);
    x = MIN(x, kScreenWidth);

    int y = MAX(point.y, 0);
    y = MIN(y, kScreenHeight);

    return (Point){ x, y };
    
} // clampPointToScreen


Triangle clampTriangleToScreen(Triangle triangle) {

    // Horizontal
    int maxX = 0;
    maxX = MAX(maxX, triangle.p1.x);
    maxX = MAX(maxX, triangle.p2.x);
    maxX = MAX(maxX, triangle.p3.x);

    if (maxX >= kScreenWidth) {
        int diff = maxX - kScreenWidth;
        triangle.p1.x -= diff;
        triangle.p2.x -= diff;
        triangle.p3.x -= diff;
    }

    int minX = INT_MAX;
    minX = MIN(minX, triangle.p1.x);
    minX = MIN(minX, triangle.p2.x);
    minX = MIN(minX, triangle.p3.x);

    if (minX < 0) {
        int diff = abs(minX);
        triangle.p1.x += diff;
        triangle.p2.x += diff;
        triangle.p3.x += diff;
    }

    // Vertical
    int maxY = 0;
    maxY = MAX(maxY, triangle.p1.y);
    maxY = MAX(maxY, triangle.p2.y);
    maxY = MAX(maxY, triangle.p3.y);

    if (maxY >= kScreenHeight) {
        int diff = maxY - kScreenHeight;
        triangle.p1.y -= diff;
        triangle.p2.y -= diff;
        triangle.p3.y -= diff;
    }

    int minY = INT_MAX;
    minY = MIN(minY, triangle.p1.y);
    minY = MIN(minY, triangle.p2.y);
    minY = MIN(minY, triangle.p3.y);

    if (minY < 0) {
        int diff = abs(minY);
        triangle.p1.y += diff;
        triangle.p2.y += diff;
        triangle.p3.y += diff;
    }

    return triangle;

} // clampTriangleToScreen


static Triangle triangleAt(Point centeredAt, int size, int rotation) {
    Triangle triangle;

    float angle;

    angle = degreesToRadians(rotation);
    triangle.p1 = (Point) { cos(angle) * size, sin(angle) * size };

    angle = degreesToRadians(rotation + 120);
    triangle.p2 = (Point) { cos(angle) * size, sin(angle) * size };

    angle = degreesToRadians(rotation + 240);
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

    Triangle clampedTriangle = clampTriangleToScreen(triangle);

    pd->graphics->fillTriangle(clampedTriangle.p1.x, clampedTriangle.p1.y,
                               clampedTriangle.p2.x, clampedTriangle.p2.y,
                               clampedTriangle.p3.x, clampedTriangle.p3.y,
                               color);
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
    demo->triangleSize = 24;
    demo->triangleAngle = 0;

    demo->speed = 1;

    return (DemoSample *)demo;
} // drawingDemoSample


