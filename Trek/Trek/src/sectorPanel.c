#include "sectorPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static Size _naturalSize(Panel *panel) {
    return (Size){ 200, 150 };
} // _naturalSize


static bool _draw(Panel *panel) {
    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    Point point = (Point){ 20, 30 };;
    drawCString("Snorgle", point);
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
