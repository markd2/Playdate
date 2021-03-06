#include <stdlib.h>
#include "pd_api.h"

typedef enum DemoSampleCategory {
    kDrawing,
    kBitmap,
    kFont,
    kSprite,
    kAudio,
} DemoSampleCategory;

struct DemoSample;

typedef const char *MenuStringCallback(struct DemoSample *sample);

// To use, embed this as the first field of your structure.
// Then provide a function make one.  Call demoSampleNew to create it,
// passing sizeof your struct.
typedef struct DemoSample {
    const char *name;
    DemoSampleCategory category;
    PDCallbackFunction *updateCallback;
    MenuStringCallback *menuStringCallback; // nullable
    // suspend
    // resume
} DemoSample;

// Zeroes out the allocated memory.
DemoSample *demoSampleNew(const char *name,
                          DemoSampleCategory category,
                          PDCallbackFunction updateCallback,
                          size_t allocSize);
void demoSampleDelete(DemoSample *sample);
