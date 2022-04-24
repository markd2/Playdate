#ifndef BORK_GEOMETRY_H
#define BORK_GEOMETRY_H

#include <stdlib.h>
#include <stdbool.h>

#ifndef PLAYDATEAPI_H
#include "pd_api.h"
#endif

#if !defined(MAX)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#if !defined(MIN)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

typedef struct Point {
    int x;
    int y;
} Point;
Point pointPlus(Point thing1, Point thing2);

typedef struct Rect {
    int x;
    int y;
    int width;
    int height;
} Rect;

typedef struct Triangle {
    Point p1;
    Point p2;
    Point p3;
} Triangle;
Triangle triangleAt(Point centeredAt, int size, int rotation);

float degreesToRadians(float degrees);

Rect screenRect(void);
Rect clampRectToScreen(Rect rect);
Point clampPointToScreen(Point point);
Triangle clampTriangleToScreen(Triangle triangle);
Rect insetRect(Rect source, int xInset, int yInset);

bool rectsIntersect(Rect thing1, Rect thing2);
void fillRect(Rect rect, LCDColor color);
void frameRect(Rect rect, LCDColor color);
Rect rectUnion(Rect thing1, Rect thing2);

LCDRect rectToLCDRect(Rect rect);

#endif // BORK_GEOMETRY_H
