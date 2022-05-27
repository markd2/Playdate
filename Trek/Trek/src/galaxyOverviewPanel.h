// galaxy overview panel

#ifndef GALAXY_OVERVIEW_PANEL_INCLUDED
#define GALAXY_OVERVIEW_PANEL_INCLUDED

#include "panel.h"
#include "trek.h" // for Galaxy

typedef struct GalaxyOverviewPanel {
    Panel panel;
    
    Galaxy *galaxy; // not owned
     
} GalaxyOverviewPanel;

GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy);
void galaxyOverviewPanelFree(Galaxy *galaxy);


#endif // GALAXY_OVERVIEW_PANEL_INCLUDED
