#include "galaxyOverviewPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static Size _naturalSize(Panel *panel) {
    int sectorWidth = 4 * 10 + 3;
    int sectorHeight = 19;
    // +1 for the width is to add a bit of margin on the left side of the text
    return (Size){ kGalaxyColumns * sectorWidth + 1, kGalaxyRows * sectorHeight };
} // _naturalSize


static bool _draw(Panel *panel) {
    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    GalaxyOverviewPanel *gopanel = (GalaxyOverviewPanel *)panel;
    pd->graphics->setFont(gopanel->font);

    char line[1024];
    char *lineScan;

    int X = 2;
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
        pd->graphics->drawText(line, strlen(line), kASCIIEncoding, X, Y);

        Y += 19;
    }
    return kUpdateDisplay;
} // _draw


GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy, LCDFont *font) {
    GalaxyOverviewPanel *panel = (GalaxyOverviewPanel *)panelNew(sizeof(GalaxyOverviewPanel));
    panel->galaxy = galaxy;
    panel->font = font;

    panel->panel.naturalSize = _naturalSize;
    panel->panel.draw = _draw;

    return panel;
    
} // galaxyOverviewPanelNew


void galaxyOverviewPanelFree(Galaxy *galaxy) {
    panelFree((Panel *)galaxy);
} // galaxyOverviewPanelFree
