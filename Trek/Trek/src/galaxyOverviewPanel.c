#include "galaxyOverviewPanel.h"

static Size _naturalSize(Panel *panel) {
    return (Size){ 150, 45 };
} // _naturalSize

static bool _draw(Panel *panel) {
    Rect rect = Rect { 0, 0, 150, 45 };
    strokeRect(rect, kColorBlack);
    return kUpdateDisplay;
} // _draw


GalaxyOverviewPanel *galaxyOverviewPanelNew(Galaxy *galaxy) {
    GalaxyOverviewPanel *panel = (GalaxyOverviewPane *)panelNew(sizeof(GalaxyOverviewPanel));
    panel->galaxy = galaxy;

    panel->naturalSize = _naturalSize;
    panel->draw = _draw;

    return panel;
    
} // galaxyOverviewPanelNew

void galaxyOverviewPanelFree(Galaxy *galaxy);
