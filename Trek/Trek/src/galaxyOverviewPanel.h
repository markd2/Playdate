// galaxy overview panel

#ifndef GALAXY_OVERVIEW_PANEL_INCLUDED
#define GALAXY_OVERVIEW_PANEL_INCLUDED

#include "panel.h"

typedef struct GalaxyOverviewPanel {
    Panel panel;
    
    Galaxy *galaxy; // not owned
     
} GalaxyOverviewPanel;

GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy);
void galaxyOverviewPanelFree(Galaxy *galaxy);


#endif // GALAXY_OVERVIEW_PANEL_INCLUDED
