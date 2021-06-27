#include <stdlib.h>

#include <math.h> // sin
#include <limits.h> // INT_MAX

#include "geometry.h"
#include "globals.h"

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


