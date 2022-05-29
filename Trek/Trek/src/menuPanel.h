// user interaction menu panel

#ifndef MENU_PANEL_INCLUDED
#define MENU_PANEL_INCLUDED

#include "panel.h"

typedef struct MenuPanel {
    Panel panel;
} MenuPanel;

MenuPanel *menuPanelNew(void);
void menuPanelFree(MenuPanel *panel);

#endif // MENU_PANEL_INCLUDED
