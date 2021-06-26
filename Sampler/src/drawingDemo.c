#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "memory.h"
#include "spy.h"

#include "pd_api.h"

typedef struct DrawingDemo {
    DemoSample isa;
    int count;
} DrawingDemo;


static int update(void *context)  {
    DrawingDemo *demo = (DrawingDemo *)context;

    demo->count++;
    print("Snorgle %d", demo->count);

    return 1;

} // update


DemoSample *drawingDemoSample(void) {
    DrawingDemo *demo = (DrawingDemo *)demoSampleNew("Drawing", kDrawing, update, sizeof(DrawingDemo));
    demo->count = 0;

    return (DemoSample *)demo;
} // drawingDemoSample


