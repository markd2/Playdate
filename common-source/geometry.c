#include <stdlib.h>

#include <math.h> // sin
#include <limits.h> // INT_MAX

#include "geometry.h"
#include "globals.h"

float degreesToRadians(float degrees) {
    float radians = ((float)M_PI * degrees) / (float)180.0;
    return radians;
} // degreesToRadians


float radiansToDegrees(float radians) {
    float degrees = radians * ((float)M_PI * 180);
    return degrees;
} // radiansToDegrees


Point pointPlus(Point thing1, Point thing2) {
    Point point = (Point) { thing1.x + thing2.x, thing1.y + thing2.y };
    return point;
} // pointPlus


Rect screenRect(void) {
    Rect rect = { 0, 0, kScreenWidth, kScreenHeight };
    return rect;
} // screenRect;


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


Rect rectInset(Rect source, int xInset, int yInset) {
    Rect rect = source;

    rect.x += xInset;
    rect.width -= 2 * xInset;
    rect.y += yInset;
    rect.height -= 2 * yInset;

    return rect;

} // rectInset


Triangle triangleAt(Point centeredAt, int size, int rotation) {
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


Point rectTopLeft(Rect rect) {
    return (Point){ rect.x, rect.y };
} // rectTopleft


Point rectBottomRight(Rect rect) {
    return (Point){ rect.x + rect.width, rect.y + rect.height };
} // rectBottomRight


bool rectsIntersect(Rect thing1, Rect thing2) {
    Point thing1TopLeft = rectTopLeft(thing1);
    Point thing1BottomRight = rectBottomRight(thing1);
    
    Point thing2TopLeft = rectTopLeft(thing2);
    Point thing2BottomRight = rectBottomRight(thing2);

    // if either is a line, then defining it can't intersect.
    if (thing1TopLeft.x == thing1BottomRight.x
        || thing1TopLeft.y == thing1BottomRight.y
        || thing2TopLeft.x == thing2BottomRight.x
        || thing2TopLeft.y == thing2BottomRight.y) {
        return false;
    }

    // see if one rectangle is totally to the left of the other
    if (thing1BottomRight.x < thing2TopLeft.x
        || thing2BottomRight.x < thing1TopLeft.x) {
        return false;
    }
    
    // see if one rectangle is totally above the other
    if (thing1BottomRight.y < thing2TopLeft.y
        || thing2BottomRight.y < thing1TopLeft.y) {
        return false;
    }

    // otherwise, they must intersect

    return true;
} // rectsIntersect


Rect rectUnion(Rect thing1, Rect thing2) {
    Point thing1TopLeft = rectTopLeft(thing1);
    Point thing1BottomRight = rectBottomRight(thing1);
    
    Point thing2TopLeft = rectTopLeft(thing2);
    Point thing2BottomRight = rectBottomRight(thing2);

    int minX = MIN(thing1TopLeft.x, thing2BottomRight.x);
    int maxX = MAX(thing1TopLeft.x, thing2BottomRight.x);

    int minY = MIN(thing1TopLeft.y, thing2BottomRight.y);
    int maxY = MAX(thing1TopLeft.y, thing2BottomRight.y);

    Rect rect = (Rect){ minX, minY, maxX - minX, maxY - minY };
    return rect;

} // rectUnion


LCDRect rectToLCDRect(Rect rect) {
    return (LCDRect){ rect.x, rect.y, rect.x + rect.width, rect.y + rect.height };
} // rectToLCDRect


Point rectCenter(Rect rect) {
    Point point = (Point){ rect.x + rect.width / 2, rect.y + rect.height / 2};
    return point;
    
} // rectCenter


Rect rectCenteredIn(Rect outerRect, Rect innerRect) {
    Point outerCenter = rectCenter(outerRect);
    Point innerCenter = rectCenter(innerRect);

    Rect result = innerRect;
    result.x += outerCenter.x - innerCenter.x;
    result.y += outerCenter.y - innerCenter.y;

    Point resultCenter = rectCenter(result);

    return result;
    
} // rectCenteredIn
