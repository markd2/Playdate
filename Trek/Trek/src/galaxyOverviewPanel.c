#include "galaxyOverviewPanel.h"
#include "drawhelpers.h"
#include "globals.h"

typedef enum DigitPosition {
    kKlingonPosition,
    kBasePosition,
    kStarPosition,
    kDelimiterPosition
} DigitPosition;

Point cellOrigin(int row, int column, DigitPosition position);

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
    Galaxy *galaxy = gopanel->galaxy;

    for (int row = 0; row < kGalaxyRows; row++) {
        for (int column = 0; column < kGalaxyColumns; column++) {

            pd->graphics->setDrawMode(kDrawModeCopy);

            // always draw the interior separators
            Point origin;
            if (column != kGalaxyColumns - 1) {
                origin = cellOrigin(row, column, kDelimiterPosition);
                drawCString(":", origin);
            }

            Sector sector = galaxy->sectors[row][column];

            bool drawHighlighted = coordinateEqual(galaxy->enterpriseSector, (Coordinate){row, column});

            if (drawHighlighted) {
                origin = cellOrigin(row, column, kKlingonPosition);
                // +1/-1's are to embiggen the inverted rectangle a smidge to give
                // a readable border.
                Rect rect = (Rect){ origin.x - 1, origin.y - 1, 3 * 11 + 1, 14 + 1 };
                fillRect(rect, kColorBlack);
                pd->graphics->setDrawMode(kDrawModeInverted);
            }

            if (!galaxy->visible[row][column]) {
                continue;
            }

            char charString[2] = {0};

            charString[0] = sector.klingonCount + '0';
            origin = cellOrigin(row, column, kKlingonPosition);
            drawCString(charString, origin);

            origin = cellOrigin(row, column, kBasePosition);
            charString[0] = sector.baseCount + '0';
            drawCString(charString, origin);

            origin = cellOrigin(row, column, kStarPosition);
            charString[0] = sector.starCount + '0';
            drawCString(charString, origin);
        }
    }
    
    return kUpdateDisplay;
} // _draw

Point cellOrigin(int row, int column, DigitPosition position) {
    int baseX = column * 11 * 4;
    int x = baseX + position * 11 + 2;
    int y = row * 19 + 2; // +2 for the top margin
    return (Point){ x, y };
} // cellOrigin


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
