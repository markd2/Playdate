// panel.h - A view.

#ifndef PANEL_INCLUDED
#define PANEL_INCLUDED

#include <stdbool.h>
#include "geometry.h" // for Size

const bool kUpdateDisplay = true;
const bool kDontUpdateDisplay = false;

typedef struct Panel {
    Size (*naturalSize)(void);
    bool (*draw)(void);  // return 1 to update display, 0 to not
} Panel;

Size panelNaturalSize(Panel *panel);
bool panelDraw(Panel *panel);

Panel *panelNew(size_t size);
void panelFree(Panel *panel);

#endif // PANEL_INCLUDED
