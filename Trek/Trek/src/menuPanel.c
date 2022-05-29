#include "menuPanel.h"
#include "globals.h"

#include "drawhelpers.h"

const int width = 190;
const int height = 210;

static Size _naturalSize(Panel *panel) {
    return (Size){ width, height };
} // _naturalSize

static bool _draw(Panel *panel) {
    Size naturalSize = panelNaturalSize(panel);
    Rect drawingArea = (Rect){ 0, 0, naturalSize.width, naturalSize.height };
    fillRect(drawingArea, kColorWhite);

    return kUpdateDisplay;
    
} // _draw

MenuPanel *menuPanelNew(void) {
    MenuPanel *panel = (MenuPanel *)panelNew(sizeof(MenuPanel));

    panel->panel.naturalSize = _naturalSize;
    panel->panel.draw = _draw;

    return panel;
    
} // menuPanelNew


void menuPanelFree(MenuPanel *panel) {
    panelFree((Panel *)panel);
} // menuPanelFree
