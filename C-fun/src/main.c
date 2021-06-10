#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static PlaydateAPI *pd = NULL;
static LCDFont *font;

static const int kScreenWidth = LCD_COLUMNS;
static const int kScreenHeight = LCD_ROWS;

static const int kTextWidth = 86;
static const int kTextHeight = 16;


int x = (kScreenWidth - kTextWidth) / 2;
int y = (kScreenHeight - kTextHeight) / 2;
int dx = 1;
int dy = 2;

static int update(void *userdata) {
    pd->graphics->clear(kColorBlack);
    pd->graphics->drawText("Greeble Bork!", strlen("Greeble Bork!"), kASCIIEncoding, x, y);

    x += dx; y += dy;
	
    if (x < 0 || x > kScreenWidth - kTextWidth) {
        dx = -dx;
    }
	
    if (y < 0 || y > kScreenHeight - kTextHeight) {
        dy = -dy;
    }
        
    pd->system->drawFPS(0,0);

    return 1;
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event, 
                 uint32_t arg) {
    if (event == kEventInit) {
        pd = playdate;

        pd->display->setRefreshRate(20);
        pd->system->setUpdateCallback(update, NULL);

        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);
    }
	
    return 0;
} // eventHandler

