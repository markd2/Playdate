#ifndef BORK_DRAWHELPERS_H
#define BORK_DRAWHELPERS_H

#include <stdlib.h>

#ifndef PLAYDATEAPI_H
#include "pd_api.h"
#endif

#include "geometry.h"

void strokeRect(Rect rect, LCDColor withColor);

#endif // BORK_DRAWHELPERS_H
