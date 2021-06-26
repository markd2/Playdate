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
    ButtonPumper *pumper;
    int count;
    
} DrawingDemo;


static int update(void *context)  {
    DrawingDemo *demo = (DrawingDemo *)context;

    demo->count++;

    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(demo->pumper, pushed, released);

    return 1;

} // update


static void handleButtons(PDButtons buttons, UpDown upDown) {
    print("buttons!  %x", buttons);
} // handleButtons


DemoSample *drawingDemoSample(void) {
    DrawingDemo *demo = (DrawingDemo *)demoSampleNew("Drawing", kDrawing, 
                                                     update,
                                                     sizeof(DrawingDemo));
    demo->pumper = buttonPumperNew(handleButtons);
    demo->count = 0;

    return (DemoSample *)demo;
} // drawingDemoSample


