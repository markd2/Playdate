#include "demoSample.h"
#include "memory.h"

DemoSample *demoSampleNew(const char *name,
                          DemoSampleCategory category,
                          PDCallbackFunction updateCallback,
                          size_t allocSize) {
    DemoSample *moi = pdMalloc(allocSize);

    moi->name = name;
    moi->category = category;
    moi->updateCallback = updateCallback;
    moi->menuImageCallback = NULL;

    return moi;

} // demoSampleNew


void demoSampleDelete(DemoSample *sample) {
    pdFree(sample);
} // demoSampleDelete
