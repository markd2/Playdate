#include <stdio.h>
#include <stdlib.h>

#include "demoSample.h"
#include "buttonpumper.h"
#include "globals.h"
#include "memory.h"
#include "spy.h"

#include "pd_api.h"

static int blah;

static int update(void *context)  {
    print("update %d", blah++);
    return 1;
} // update


DemoSample *drawingDemoSample(void) {
    return demoSampleNew("Drawing", kDrawing, update);
} // drawingDemoSample


