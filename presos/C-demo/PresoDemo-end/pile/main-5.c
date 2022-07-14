#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

PlaydateAPI* pd;
#define print pd->system->logToConsole


typedef struct GameData {
    PDRect rect;
} GameData;



static GameData gameData;

static int update(void *userdata) {
    GameData *gameData = userdata;

    pd->graphics->clear(kColorWhite);

    pd->graphics->drawRect(gameData->rect.x, gameData->rect.y, 
                           gameData->rect.width, gameData->rect.height, kColorBlack);

    PDButtons pushedButtons;
    pd->system->getButtonState(&pushedButtons, NULL, NULL); // &pushedButtons, NULL);

    if (pushedButtons & kButtonLeft) {
        gameData->rect.x--;
    } else if (pushedButtons & kButtonRight) {
        gameData->rect.x++;
    } else if (pushedButtons & kButtonUp) {
        gameData->rect.y--;
    } else if (pushedButtons & kButtonDown) {
        gameData->rect.y++;
    }

    if (pushedButtons & kButtonA) {
        gameData->rect.width += (random() % 5);
        gameData->rect.height += (random() % 5);
    }
    if (pushedButtons & kButtonB) {
        gameData->rect.width -= (random() % 5);
        gameData->rect.height -= (random() % 5);
    }
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
        gameData.rect = PDRectMake(10, 30, 40, 40);
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
