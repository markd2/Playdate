#ifndef BORK_GLOBALS_H
#define BORK_GLOBALS_H

#include "pd_api.h"

#define print pd->system->logToConsole 
#define kScreenWidth LCD_COLUMNS
#define kScreenHeight LCD_ROWS

extern PlaydateAPI *pd;

#endif // BORK_GLOBALS_H
