#include "spy.h"
#include "globals.h"
#include "assert.h"

#include "pd_api.h"

static struct playdate_sys originalSystem;

void uninstallSpies(void) {
    *((struct playdate_sys *)pd->system) = originalSystem;
} // uninstallSpies


static void* _realloc(void* ptr, size_t size) {
    print((char *)__func__);
    return originalSystem.realloc(ptr, size);
} // _realloc


// Don't know how to forward on `...` to a non-va method, if it's possible
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
    print((char *)__func__);
    return originalSystem.getLanguage();
} // _getLanguage


static unsigned int _getCurrentTimeMilliseconds(void) {
    print((char *)__func__);
    return originalSystem.getCurrentTimeMilliseconds();
} // _getCurrentTimeMilliseconds


static unsigned int _getSecondsSinceEpoch(unsigned int *milliseconds) {
    print((char *)__func__);
    return originalSystem.getSecondsSinceEpoch(milliseconds);
} // _getSecondsSinceEpoch


static void _drawFPS(int x, int y) {
    // print((char *)__func__);
    originalSystem.drawFPS(x, y);
} // _drawFPS


static void _setUpdateCallback(PDCallbackFunction* update, void* userdata) {
    print((char *)__func__);
    originalSystem.setUpdateCallback(update, userdata);
} // _setUpdateCallback


static void _getButtonState(PDButtons* current, PDButtons* pushed, PDButtons* released) {
    // print((char *)__func__);
    originalSystem.getButtonState(current, pushed, released);
} // _getButtonState


static void _setPeripheralsEnabled(PDPeripherals mask) {
    print((char *)__func__);
    originalSystem.setPeripheralsEnabled(mask);
} // _setPeripheralsEnabled


static void _getAccelerometer(float* outx, float* outy, float* outz) {
    print((char *)__func__);
    originalSystem.getAccelerometer(outx, outy, outz);
} // _getAccelerometer


static float _getCrankChange(void) {
    print((char *)__func__);
    return originalSystem.getCrankChange();
} // _getCrankChange


static float _getCrankAngle(void) {
    print((char *)__func__);
    return originalSystem.getCrankAngle();
} // _getCrankAngle


static int _isCrankDocked(void) {
    print((char *)__func__);
    return originalSystem.isCrankDocked();
} // _isCrankDocked


static int _setCrankSoundsDisabled(int flag) {
    print((char *)__func__);
    return originalSystem.setCrankSoundsDisabled(flag);
} // _setCrankSoundsDisabled


static int _getFlipped(void) {
    print((char *)__func__);
    return originalSystem.getFlipped();
} // _getFlipped


static void _setAutoLockDisabled(int enable) {
    print((char *)__func__);
    originalSystem.setAutoLockDisabled(enable);
} // _setAutoLockDisabled


static void _setMenuImage(LCDBitmap* bitmap, int xOffset) {
    print((char *)__func__);
    originalSystem.setMenuImage(bitmap, xOffset);
} // _setMenuImage


static PDMenuItem* _addMenuItem(const char *title, PDMenuItemCallbackFunction* callback, void* userdata) {
    print((char *)__func__);
    return originalSystem.addMenuItem(title, callback, userdata);
} // _addMenuItem


static PDMenuItem* _addCheckmarkMenuItem(const char *title, int value, PDMenuItemCallbackFunction* callback, void* userdata) {
    print((char *)__func__);
    return originalSystem.addCheckmarkMenuItem(title, value, callback, userdata);
} // _addCheckmarkMenuItem


static PDMenuItem* _addOptionsMenuItem(const char *title, const char** optionTitles, int optionsCount, PDMenuItemCallbackFunction* f, void* userdata) {
    print((char *)__func__);
    return originalSystem.addOptionsMenuItem(title, optionTitles, optionsCount, f, userdata);
} // _addOptionsMenuItem


static void _removeAllMenuItems(void) {
    print((char *)__func__);
    originalSystem.removeAllMenuItems();
} // _removeAllMenuItems


static void _removeMenuItem(PDMenuItem *menuItem) {
    print((char *)__func__);
    originalSystem.removeMenuItem(menuItem);
} // _removeMenuItem


static int _getMenuItemValue(PDMenuItem *menuItem) {
    print((char *)__func__);
    return originalSystem.getMenuItemValue(menuItem);
} // _getMenuItemValue


static void _setMenuItemValue(PDMenuItem *menuItem, int value) {
    print((char *)__func__);
    originalSystem.setMenuItemValue(menuItem, value);
} // _setMenuItemValue


static const char* _getMenuItemTitle(PDMenuItem *menuItem) {
    print((char *)__func__);
    return originalSystem.getMenuItemTitle(menuItem);
} // _getMenuItemTitle


static void _setMenuItemTitle(PDMenuItem *menuItem, const char *title) {
    print((char *)__func__);
    originalSystem.setMenuItemTitle(menuItem, title);
} // _setMenuItemTitle


static void* _getMenuItemUserdata(PDMenuItem *menuItem) {
    print((char *)__func__);
    return originalSystem.getMenuItemUserdata(menuItem);
} // _getMenuItemUserdata


static void _setMenuItemUserdata(PDMenuItem *menuItem, void *ud) {
    print((char *)__func__);
    originalSystem.setMenuItemUserdata(menuItem, ud);
} // _setMenuItemUserdata


static int _getReduceFlashing(void) {
    print((char *)__func__);
    return originalSystem.getReduceFlashing();
} // _getReduceFlashing


// --------------------------------------------------

void installSpies(void) {

    // prevent multiple installs - end up with infinite recursion.
    static int installed = 0;
    if (installed) return;
    installed = 1;

    originalSystem = *pd->system;
    
    struct playdate_sys *sys = ((struct playdate_sys*)(pd->system));

    sys->realloc = _realloc;
    sys->getLanguage = _getLanguage;
    sys->getCurrentTimeMilliseconds = _getCurrentTimeMilliseconds;
    sys->getSecondsSinceEpoch = _getSecondsSinceEpoch;
    sys->drawFPS = _drawFPS;
    sys->setUpdateCallback = _setUpdateCallback;
    sys->getButtonState = _getButtonState;
    sys->setPeripheralsEnabled = _setPeripheralsEnabled;
    sys->getAccelerometer = _getAccelerometer;
    sys->getCrankChange = _getCrankChange;
    sys->getCrankAngle = _getCrankAngle;
    sys->isCrankDocked = _isCrankDocked;
    sys->setCrankSoundsDisabled = _setCrankSoundsDisabled;
    sys->getFlipped = _getFlipped;
    sys->setAutoLockDisabled = _setAutoLockDisabled;
    sys->setMenuImage = _setMenuImage;
    sys->addMenuItem = _addMenuItem;
    sys->addCheckmarkMenuItem = _addCheckmarkMenuItem;
    sys->addOptionsMenuItem = _addOptionsMenuItem;
    sys->removeAllMenuItems = _removeAllMenuItems;
    sys->removeMenuItem = _removeMenuItem;
    sys->getMenuItemValue = _getMenuItemValue;
    sys->setMenuItemValue = _setMenuItemValue;
    sys->getMenuItemTitle = _getMenuItemTitle;
    sys->setMenuItemTitle = _setMenuItemTitle;
    sys->getMenuItemUserdata = _getMenuItemUserdata;
    sys->setMenuItemUserdata = _setMenuItemUserdata;
    sys->getReduceFlashing = _getReduceFlashing;

} // installSpies
