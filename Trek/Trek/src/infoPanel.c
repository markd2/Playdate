#include "infoPanel.h"
#include "drawhelpers.h"
#include "globals.h"

static Size _naturalSize(Panel *panel) {
    return (Size){ 370, 210 };
} // _naturalSize


static bool _draw(Panel *panel) {
    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    return kUpdateDisplay;
} // _draw


InfoPanel *infoPanelNew(Galaxy *galaxy, LCDFont *font) {
    InfoPanel *panel = (InfoPanel *)panelNew(sizeof(InfoPanel));
    panel->galaxy = galaxy;
    panel->font = font;

    panel->panel.naturalSize = _naturalSize;
    panel->panel.draw = _draw;

    return panel;
    
} // infoPanelNew


void infoPanelFree(InfoPanel *panel) {
    panelFree((Panel *)panel);
} // infoPanelFree
