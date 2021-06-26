#include <stdlib.h>
#include "pd_api.h"

typedef enum DemoSampleCategory {
    kDrawing,
    kSprint,
    kSynth
} DemoSampleCategory;

// To use, embed this as the first field of your structure.
// Then provide a function make one.  Call demoSampleNew to create it,
// passing sizeof your struct.
typedef struct DemoSample {
    const char *name;
    DemoSampleCategory category;
    PDCallbackFunction *updateCallback;
    // suspend
    // resume
} DemoSample;

DemoSample *demoSampleNew(const char *name,
                          DemoSampleCategory category,
                          PDCallbackFunction updateCallback,
                          size_t allocSize);
void demoSampleDelete(DemoSample *sample);
