// main info panel (quadrant and ship status)

#ifndef INFO_PANEL_INCLUDED
#define INFO_PANEL_INCLUDED

#include "panel.h"
#include "trek.h" // for Galaxy
#include "globals.h" // for LCDFont

typedef struct InfoPanel {
    Panel panel;
    Galaxy *galaxy; // not owned
    LCDFont *font;  // not owned
} InfoPanel;

InfoPanel *infoPanelNew(Galaxy *galaxy, LCDFont *font);
void infoPanelFree(InfoPanel *panel);

#endif // INFO_PANEL_INCLUDED
