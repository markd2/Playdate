#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static PlaydateAPI *pd = NULL;
static LCDFont *font;

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400 - TEXT_WIDTH) / 2;
int y = (240 - TEXT_HEIGHT) / 2;
int dx = 1;
int dy = 2;

// static int update(__attribute__ ((unused)) void* ud)
static int update(void *ud) {
    pd->graphics->clear(kColorBlack);
    pd->graphics->drawText("Greeble Bork!", strlen("Greeble Bork!"), kASCIIEncoding, x, y);

    x += dx; y += dy;
	
    if ( x < 0 || x > LCD_COLUMNS - TEXT_WIDTH ) {
        dx = -dx;
    }
	
    if ( y < 0 || y > LCD_ROWS - TEXT_HEIGHT ) {
        dy = -dy;
    }
        
    pd->system->drawFPS(0,0);

    return 1;
} // update


int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event, 
                 uint32_t arg) {
    if ( event == kEventInit ) {
        pd = playdate;

        pd->display->setRefreshRate(20);
        pd->system->setUpdateCallback(update, NULL);

        font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        pd->graphics->setFont(font);
    }
	
    return 0;
} // eventHandler

