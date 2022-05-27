#include "panel.h"
#include "memory.h"

Size panelNaturalSize(Panel *panel) {
    return panel->naturalSize();
} // panelNaturalSize


bool panelDraw(Panel *panel) {
    return panel->draw();
} // panelDraw


Panel *panelNew(size_t size) {
    return pdMalloc(size);
} // panelNew


void panelFree(Panel *panel) {
    pdFree(panel);
} // panelFree
