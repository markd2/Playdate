#ifndef BORK_GLOBALS_H
#define BORK_GLOBALS_H

#ifndef PLAYDATEAPI_H
#include "pd_api.h"
#endif

#define print pd->system->logToConsole 
#define kScreenWidth LCD_COLUMNS // 400
#define kScreenHeight LCD_ROWS   // 240

extern PlaydateAPI *pd;

extern const char *eventNames[];

#endif // BORK_GLOBALS_H
