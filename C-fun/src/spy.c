#include "spy.h"
#include "globals.h"
#include "assert.h"

#include "pd_api.h"

static struct playdate_sys originalSystem;

void uninstallSpies(void) {
    *((struct playdate_sys *)pd->system) = originalSystem;
} // uninstallSpies


static void _removeAllMenuItems(void) {
    print("REMOVE ALL");
    originalSystem.removeAllMenuItems();
} // _removeAllMenuItems


void installSpies(void) {

    // prevent multiple installs - end up with infinite recursion.
    static int installed = 0;
    if (installed) return;
    installed = 1;

    originalSystem = *pd->system;
    ((struct playdate_sys*)(pd->system))->removeAllMenuItems = _removeAllMenuItems;

} // installSpies

