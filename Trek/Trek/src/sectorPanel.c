#include "sectorPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static const int kCharacterWidth = 10;
static const int kCharacterHeight = 14;


static Size _naturalSize(Panel *panel) {
    // sector
    int sectorWidth = kCharacterWidth * kSectorColumns * 2 + kSectorColumns;
    int sectorHeight = kCharacterHeight * kSectorRows + kSectorRows;

    // labels
    sectorWidth += kCharacterWidth;
    sectorHeight += kCharacterHeight;

    return (Size){ sectorWidth, sectorHeight };
} // _naturalSize


static void drawSector(void) {
    Point origin = { 0 };
    char buffer[16];

    for (int row = 0; row < kSectorRows; row++) {
        origin.x = 0;
        for (int column = 0 ; column < kSectorColumns; column++) {
            drawCString("# ", origin);
            origin.x += kCharacterWidth * 2 + 2;
        }

        origin.x -= kCharacterWidth + 1;
        drawCStringf(origin, buffer, sizeof(buffer), "%d", row + 1);

        origin.y += kCharacterHeight + 1;
    }
    origin.x = 0;
    drawCString("1 2 3 4 5 6 7 8", origin);
} // drawSector


static void drawMenu(void) {
} // drawMenu

static bool _draw(Panel *panel) {
    SectorPanel *sectorPanel = (SectorPanel *)panel;

    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    Point point = (Point){ 20, 30 };;

    pd->graphics->setFont(sectorPanel->font);

    drawSector();

    return true;
} // _draw


SectorPanel *sectorPanelNew(SectorDetail *sector, LCDFont *font) {
    SectorPanel *panel = (SectorPanel *)panelNew(sizeof(SectorPanel));
    panel->sector = sector;
    panel->font = font;

    panel->panel.naturalSize = _naturalSize;
    panel->panel.draw = _draw;

    return panel;
    
} // sectorPanelNew

void sectorPanelFree(SectorPanel *panel) {
    panelFree((Panel *)panel);
} // sectorPanelFree
