#include "demoSample.h"
#include "memory.h"

DemoSample *demoSampleNew(const char *name,
                          DemoSampleCategory category,
                          PDCallbackFunction updateCallback) {
    DemoSample *moi = pdMalloc(sizeof(DemoSample));

    moi->name = name;
    moi->category = category;
    moi->updateCallback = updateCallback;

    return moi;

} // demoSampleNew


void demoSampleDelete(DemoSample *sample) {
    pdFree(sample);
} // demoSampleDelete
