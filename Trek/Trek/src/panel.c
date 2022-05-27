#include "panel.h"
#include "memory.h"

const bool kUpdateDisplay = true;
const bool kDontUpdateDisplay = false;


Size panelNaturalSize(Panel *panel) {
    return panel->naturalSize(panel);
} // panelNaturalSize


bool panelDraw(Panel *panel) {
    return panel->draw(panel);
} // panelDraw


static Size _naturalSize(Panel *panel) {
    return (Size){ 0, 0 };
} // _naturalSize

static bool _draw(Panel *panel) {
    return kDontUpdateDisplay;
} // _draw


Panel *panelNew(size_t size) {
    Panel *panel = pdMalloc(size);
    panel->naturalSize = _naturalSize;
    panel->draw = _draw;

    return panel;
} // panelNew


void panelFree(Panel *panel) {
    pdFree(panel);
} // panelFree
