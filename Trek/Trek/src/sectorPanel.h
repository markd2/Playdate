// sector overview panel

#ifndef SECTOR_PANEL_INCLUDED
#define SECTOR_PANEL_INCLUDED

#include "panel.h"
#include "trek.h"

typedef struct SectorPanel {
    Panel panel;

    Galaxy *galaxy; // not owned
    LCDFont *font; // not owned
    
    Coordinate sectorCoordinate;
    SectorDetail *sector;
} SectorPanel;

SectorPanel *sectorPanelNew(SectorDetail *sector, LCDFont *font);
void sectorPanelFree(SectorPanel *panel);

#endif // SECTOR_PANEL_INCLUDED
