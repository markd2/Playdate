#include "galaxyOverviewPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static Size _naturalSize(Panel *panel) {
    int sectorWidth = 4 * 10 + 3;
    int sectorHeight = 19;
    return (Size){ kGalaxyColumns * sectorWidth, kGalaxyRows * sectorHeight };
} // _naturalSize


static bool _draw(Panel *panel) {
    GalaxyOverviewPanel *gopanel = (GalaxyOverviewPanel *)panel;
    char line[1024];
    char *lineScan;

    int Y = 2;

    for (int row = 0; row < kGalaxyRows; row++) {
        lineScan = line;
        for (int column = 0; column < kGalaxyColumns; column++) {
            Sector sector = gopanel->galaxy->sectors[row][column];
            *lineScan++ = sector.klingonCount + '0';
            *lineScan++ = sector.baseCount + '0';
            *lineScan++ = sector.starCount + '0';

            *lineScan++ = ':';
        }
        *(lineScan - 1) = '\000';
        pd->graphics->drawText(line, strlen(line), kASCIIEncoding, 1, Y);

        Y += 19;
    }
    return kUpdateDisplay;
} // _draw


GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy) {
    GalaxyOverviewPanel *panel = (GalaxyOverviewPanel *)panelNew(sizeof(GalaxyOverviewPanel));
    panel->galaxy = galaxy;

    panel->panel.naturalSize = _naturalSize;
    panel->panel.draw = _draw;

    return panel;
    
} // galaxyOverviewPanelNew

void galaxyOverviewPanelFree(Galaxy *galaxy);
