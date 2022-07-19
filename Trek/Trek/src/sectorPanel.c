#include "sectorPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static const int kCharacterWidth = 10;
static const int kCharacterHeight = 14;
static const int kMenuCharacters = 20;

static int naturalSectorWidth(void) {
    int sectorWidth = kCharacterWidth * kSectorColumns * 2 + kSectorColumns;
    sectorWidth += kCharacterWidth;
    return sectorWidth;
} // naturalSectorWidth

static Size _naturalSize(Panel *panel) {
    // sector
    int sectorWidth = naturalSectorWidth();
    int sectorHeight = kCharacterHeight * kSectorRows + kSectorRows;
    sectorHeight += kCharacterHeight;

    // menu
    sectorWidth += kCharacterWidth * kMenuCharacters + kCharacterWidth;
    sectorHeight += kCharacterHeight + 1;

    return (Size){ sectorWidth, sectorHeight };
} // _naturalSize


static void drawSector(SectorPanel *sectorPanel) {
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


static void drawMenu(SectorPanel *sectorPanel) {
    Point origin = (Point){ naturalSectorWidth() + kCharacterWidth + 1, 0 };

    const char *blah[] = {
        "YEARS       2.97",
        "STARDATE    3424.0",
        "CONDITION   RED",
        "SHIELDS     50%",
        "SHIELD NRG  2189",
        "AVAIL NRG   2359",
        "PH TORPS    10",
        "KLINGONS    41",
        "BASES       4",
        "COURSE NOT SET."
        };

    const char **scan = blah;
    const char **stop = scan + sizeof(blah) / sizeof(*blah);

    while (scan < stop) {
        drawCString(*scan, origin);
        origin.y += kCharacterHeight + 1;
        scan++;
    }
} // drawMenu


static bool _draw(Panel *panel) {
    SectorPanel *sectorPanel = (SectorPanel *)panel;

    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    Point point = (Point){ 20, 30 };;

    pd->graphics->setFont(sectorPanel->font);

    drawSector(sectorPanel);
    drawMenu(sectorPanel);

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
