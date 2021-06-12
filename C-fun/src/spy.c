#include "spy.h"
#include "globals.h"
#include "assert.h"

#include "pd_api.h"

static struct playdate_sys originalSystem;

void uninstallSpies(void) {
    *((struct playdate_sys *)pd->system) = originalSystem;
} // uninstallSpies


static void* _realloc(void* ptr, size_t size) {
    return originalSystem.realloc(ptr, size);
} // _realloc


#if 0
static int _formatString(char **ret, const char *fmt, ...) {
    originalSystem.
} // _formatString


static void _logToConsole(char* fmt, ...) {
    originalSystem.
} // _logToConsole


static void _error(const char* fmt, ...) {
    originalSystem.
} // _error
#endif


static PDLanguage _getLanguage(void) {
    return originalSystem.getLanguage();
} // _getLanguage


static unsigned int _getCurrentTimeMilliseconds(void) {
    return originalSystem.getCurrentTimeMilliseconds();
} // _getCurrentTimeMilliseconds


static unsigned int _getSecondsSinceEpoch(unsigned int *milliseconds) {
    return originalSystem.getSecondsSinceEpoch(milliseconds);
} // _getSecondsSinceEpoch


static void _drawFPS(int x, int y) {
    originalSystem.drawFPS(x, y);
} // _drawFPS


static void _setUpdateCallback(PDCallbackFunction* update, void* userdata) {
    originalSystem.setUpdateCallback(update, userdata);
} // _setUpdateCallback


static void _getButtonState(PDButtons* current, PDButtons* pushed, PDButtons* released) {
    originalSystem.getButtonState(current, pushed, released);
} // _getButtonState


static void _setPeripheralsEnabled(PDPeripherals mask) {
    originalSystem.setPeripheralsEnabled(mask);
} // _setPeripheralsEnabled


static void _getAccelerometer(float* outx, float* outy, float* outz) {
    originalSystem.getAccelerometer(outx, outy, outz);
} // _getAccelerometer


static float _getCrankChange(void) {
    return originalSystem.getCrankChange();
} // _getCrankChange


static float _getCrankAngle(void) {
    return originalSystem.getCrankAngle();
} // _getCrankAngle


static int _isCrankDocked(void) {
    return originalSystem.isCrankDocked();
} // _isCrankDocked


static int _setCrankSoundsDisabled(int flag) {
    return originalSystem.setCrankSoundsDisabled(flag);
} // _setCrankSoundsDisabled


static int _getFlipped(void) {
    return originalSystem.getFlipped();
} // _getFlipped


static void _setAutoLockDisabled(int enable) {
    originalSystem.setAutoLockDisabled(enable);
} // _setAutoLockDisabled


static void _setMenuImage(LCDBitmap* bitmap, int xOffset) {
    originalSystem.setMenuImage(bitmap, xOffset);
} // _setMenuImage


static PDMenuItem* _addMenuItem(const char *title, PDMenuItemCallbackFunction* callback, void* userdata) {
    return originalSystem.addMenuItem(title, callback, userdata);
} // _addMenuItem


static PDMenuItem* _addCheckmarkMenuItem(const char *title, int value, PDMenuItemCallbackFunction* callback, void* userdata) {
    return originalSystem.addCheckmarkMenuItem(title, value, callback, userdata);
} // _addCheckmarkMenuItem


static PDMenuItem* _addOptionsMenuItem(const char *title, const char** optionTitles, int optionsCount, PDMenuItemCallbackFunction* f, void* userdata) {
    return originalSystem.addOptionsMenuItem(title, optionTitles, optionsCount, f, userdata);
} // _addOptionsMenuItem


static void _removeAllMenuItems(void) {
    originalSystem.removeAllMenuItems();
} // _removeAllMenuItems


static void _removeMenuItem(PDMenuItem *menuItem) {
    originalSystem.removeMenuItem(menuItem);
} // _removeMenuItem


static int _getMenuItemValue(PDMenuItem *menuItem) {
    return originalSystem.getMenuItemValue(menuItem);
} // _getMenuItemValue


static void _setMenuItemValue(PDMenuItem *menuItem, int value) {
    originalSystem.setMenuItemValue(menuItem, value);
} // _setMenuItemValue


static const char* _getMenuItemTitle(PDMenuItem *menuItem) {
    return originalSystem.getMenuItemTitle(menuItem);
} // _getMenuItemTitle


static void _setMenuItemTitle(PDMenuItem *menuItem, const char *title) {
    originalSystem.setMenuItemTitle(menuItem, title);
} // _setMenuItemTitle


static void* _getMenuItemUserdata(PDMenuItem *menuItem) {
    return originalSystem.getMenuItemUserdata(menuItem);
} // _getMenuItemUserdata


static void _setMenuItemUserdata(PDMenuItem *menuItem, void *ud) {
    originalSystem.setMenuItemUserdata(menuItem, ud);
} // _setMenuItemUserdata


static int _getReduceFlashing(void) {
    return originalSystem.getReduceFlashing();
} // _getReduceFlashing


// --------------------------------------------------

void installSpies(void) {

    // prevent multiple installs - end up with infinite recursion.
    static int installed = 0;
    if (installed) return;
    installed = 1;

    originalSystem = *pd->system;
    ((struct playdate_sys*)(pd->system))->removeAllMenuItems = _removeAllMenuItems;

} // installSpies

