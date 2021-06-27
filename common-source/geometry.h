#include <stdlib.h>

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

Rect clampRectToScreen(Rect rect);
Point clampPointToScreen(Point point);
Triangle clampTriangleToScreen(Triangle triangle);
