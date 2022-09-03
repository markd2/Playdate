#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI* pd;
#define print pd->system->logToConsole

typedef struct GameData {
    int x;
    int y;
    int width;
    int height;
} GameData;
static GameData gameData;

static int update(void *userdata) {
    GameData *gameData = userdata;

    pd->graphics->clear(kColorWhite);

    pd->graphics->drawRect(gameData->x, gameData->y, 
                           gameData->width, gameData->height, kColorBlack);

    gameData->x++;
    gameData->y++;

    gameData->width += (random() % 5);
    gameData->height += (random() % 5);
    
    return 1;
} // update


#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event,
                 uint32_t arg) {
    switch (event) {
    case kEventInit:
        pd = playdate;
        print("kEventInit");
        // setting this now assumes pure C ad doesn't run any Lua code
        gameData.x = 10;
        gameData.y = 30;
        gameData.width = 40;
        gameData.height = 40;
        pd->system->setUpdateCallback(update, &gameData);
        break;

    case kEventInitLua:
        print("kEventInitLua");
        break;

    case kEventLock:
        print("kEventLock");
        break;

    case kEventUnlock:
        print("kEventUnlock");
        break;

    case kEventPause:
        print("kEventPause");
        break;

    case kEventResume:
        print("kEventResume");
        break;

    case kEventTerminate:
        print("kEventTerminate");
        break;

    case kEventKeyPressed:
        print("kEventKeyPressed");
        break;

    case kEventKeyReleased:
        print("kEventKeyReleased");
        break;

    case kEventLowPower:
        print("kEventLowPower");
        break;
    }

    return 0;
} // eventHandler
