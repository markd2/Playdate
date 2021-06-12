#include "spy.h"
#include "globals.h"

static struct playdate_sys originalSystem;

void (*glurble)(void);

void _removeAllMenuItems(void) {
    print("REMOVE ALL");
    originalSystem.removeAllMenuItems();
} // removeAllMenuItems


void installSpies(void) {
    originalSystem = *pd->system;
    glurble = pd->system->removeAllMenuItems;

    ((struct playdate_sys*)(pd->system))->removeAllMenuItems = _removeAllMenuItems;

} // installSpies


