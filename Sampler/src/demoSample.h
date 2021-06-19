#include <stdlib.h>
#include "pd_api.h"

typedef enum DemoSampleCategory {
    kDrawing,
    kSprint,
    kSynth
} DemoSampleCategory;

typedef struct DemoSample {
    const char *name;
    DemoSampleCategory category;
    PDCallbackFunction *updateCallback;
    // suspend
    // resume
} DemoSample;

DemoSample *demoSampleNew(const char *name,
                          DemoSampleCategory category,
                          PDCallbackFunction updateCallback);
void demoSampleDelete(DemoSample *sample);
