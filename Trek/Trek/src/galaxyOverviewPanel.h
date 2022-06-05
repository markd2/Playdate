// galaxy overview panel

#ifndef GALAXY_OVERVIEW_PANEL_INCLUDED
#define GALAXY_OVERVIEW_PANEL_INCLUDED

#include "panel.h"
#include "trek.h" // for Galaxy
#include "globals.h" // for LCDFont

typedef struct GalaxyOverviewPanel {
    Panel panel;
    
    Galaxy *galaxy; // not owned
    LCDFont *font;  // not owned
     
} GalaxyOverviewPanel;

GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy, LCDFont *font);
void galaxyOverviewPanelFree(GalaxyOverviewPanel *panel);


#endif // GALAXY_OVERVIEW_PANEL_INCLUDED
