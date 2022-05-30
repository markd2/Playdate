#include <stdio.h>
#include <stdlib.h>

#include "buttonpumper.h"
#include "globals.h"

#include "pd_api.h"

#include "drawhelpers.h"
#include "galaxyOverviewPanel.h"
#include "menuPanel.h"
#include "panel.h"
#include "patterns.h"
#include "trek.h"


static ButtonPumper *pumper;
static LCDFont *appleFont;

static const int kVerticalDrawingOffset = kScreenHeight / 2 - 50;

static Panel *panel;
static Panel *menuPanel;
static Panel *overlayPanel; // null for no overlay

static Galaxy galaxy;


static void handleButtons(PDButtons buttons, UpDown upDown, void *context) {
    if (buttons == kButtonB && upDown == kPressed) {
        overlayPanel = menuPanel;
    } else if (buttons == kButtonB) {
        overlayPanel = NULL;
    }

    if (upDown == kPressed) {
        switch (buttons) {
        case kButtonLeft:
            galaxy.enterpriseSector.column--;
            break;
        case kButtonRight:
            galaxy.enterpriseSector.column++;
            break;
        case kButtonUp:
            galaxy.enterpriseSector.row--;
            break;
        case kButtonDown:
            galaxy.enterpriseSector.row++;
            break;
        default:
            break;
        }
    }

    if (galaxy.enterpriseSector.column < 0) galaxy.enterpriseSector.column = kGalaxyColumns - 1;
    if (galaxy.enterpriseSector.column >= kGalaxyColumns) galaxy.enterpriseSector.column = 0;

    if (galaxy.enterpriseSector.row < 0) galaxy.enterpriseSector.row = kGalaxyRows - 1;
    if (galaxy.enterpriseSector.row >= kGalaxyRows) galaxy.enterpriseSector.row = 0;

    galaxyMakeVisibleAroundSector(&galaxy, galaxy.enterpriseSector);

} // handleButtons


static void draw(LCDFont *font, const char *string) {

    Size panelSize = panelNaturalSize(panel);
    Rect panelRect = (Rect){ 0, 0, panelSize.width, panelSize.height };
    Rect screen = screenRect();

    fillRect(screen, (uintptr_t)percent50Pattern);

    Rect centeredRect = rectCenteredIn(screen, panelRect);

    pd->graphics->pushContext(NULL); {
        pd->graphics->setDrawOffset(centeredRect.x, centeredRect.y);
        panelDraw(panel);
    } pd->graphics->popContext();

    frameRect(centeredRect, kColorBlack);

    if (overlayPanel) {
        Size overlaySize = panelNaturalSize(overlayPanel);
        Rect overlayRect = (Rect){ 0, 0, overlaySize.width, overlaySize.height };
        Rect centeredOverlay = rectCenteredIn(screen, overlayRect);

        pd->graphics->pushContext(NULL); {
            pd->graphics->setDrawOffset(centeredOverlay.x, centeredOverlay.y);
            panelDraw(overlayPanel);
        } pd->graphics->popContext();

        frameRect(centeredOverlay, kColorBlack);
    }
} // draw


// --------------------------------------------------

static char *infoString = ""
" QUADRANT 3-1     SECTOR     3-4\n"
"              K1  YEARS      2.97\n"
"              K2  STARDTE    3424.0\n"
"     #E        3  CONDITION  RED\n"
"    K          4  SHIELDS    50%\n"
"              *5  SHIELD NRG 2189\n"
"               6  AVAIL NRG  2359\n"
"               7  PH TORPS   10\n"
"*              8  KLINGONS   41\n"
"1 2 3 4 5 6 7 8   BASES      4\n"
"                  COURSE NOT SET.";


static int flippa = 1;
static int updateDisplay(void) {
//    if (flippa) {
        draw(appleFont, infoString);
//        flippa = 0;
        return 1; // 1 to update the disply, 0 to naught do it.
//    } else {
//        return 0;
//    }
} // updateDisplay



// Return 1 to update the display, 0 to not update it
static int update(void *userdata) {
    PDButtons pushed, released;
    pd->system->getButtonState(NULL, &pushed, &released);
    buttonPumperPump(pumper, pushed, released);
    return updateDisplay();
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    // NOTHING GOES HERE
    pd = playdate;
    pd->system->logToConsole("event received %s (%x)", eventNames[event], arg);

    switch (event) {
    case kEventInit: {
        pd->display->setRefreshRate(10);
        
        const char *errorText = NULL;
        appleFont = pd->graphics->loadFont("font/Apple2-14",
                                           &errorText);
        if (appleFont == NULL) {
            print("no Apple ][ font");
        }
        pd->system->setUpdateCallback(update, NULL);
        pumper = buttonPumperNew(handleButtons, NULL);

        pd->system->setCrankSoundsDisabled(1);  // turn off sound

        int baseCount = 10;
        int klingonCount = 50;
        galaxyRandomize(&galaxy, baseCount, klingonCount);
        galaxyPrint(&galaxy);
        panel = (Panel *)galaxyOverviewPanelNew(&galaxy, appleFont);

        menuPanel = (Panel *)menuPanelNew();
        break;
    }

    case kEventPause:
        break;

    case kEventResume:
        break;

    case kEventTerminate:
        break;

    default: break;
    }
    
    return 0;

} // eventHandler
