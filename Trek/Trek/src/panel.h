// panel.h - A view.

#ifndef PANEL_INCLUDED
#define PANEL_INCLUDED

#include <stdbool.h>
#include "geometry.h" // for Size

extern const bool kUpdateDisplay;
extern const bool kDontUpdateDisplay;

typedef struct Panel Panel;

struct Panel {
    Size (*naturalSize)(Panel *panel);
    bool (*draw)(Panel *panel);  // return 1 to update display, 0 to not
};

Size panelNaturalSize(Panel *panel);
bool panelDraw(Panel *panel); // draw relative to (0,0), coordinate system will be changed before drawing if needed.

Panel *panelNew(size_t size);
void panelFree(Panel *panel);

#endif // PANEL_INCLUDED
