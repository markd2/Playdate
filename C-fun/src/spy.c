#include "spy.h"
#include "globals.h"

static struct playdate_sys originalSystem;

void _removeAllMenuItems(void) {
    print("REMOVE ALL");
    originalSystem.removeAllMenuItems();
} // _removeAllMenuItems


void installSpies(void) {
    static int installed = 0;

    if (installed) {
        return;
    }
    installed = 1;

    print("HELLO");
    originalSystem = *pd->system;
    ((struct playdate_sys*)(pd->system))->removeAllMenuItems = _removeAllMenuItems;

} // installSpies


/*

Having a moment of existential C-risis, involving ~swizzling~ patching some dispatch
tables.

From the vendor, have this struct

```
typedef struct
{
    const struct playdate_sys* system;
    ...
} PlaydateAPI;
```

with a pointer to this struct:

```
struct playdate_sys
{
    void* (*realloc)(void* ptr, size_t size); // ptr = NULL -> malloc, size = 0 -> free
    ...
    void (*removeAllMenuItems)(void);
```

and call it like this, with `pd` being a pointer to the globally vended PlaydateAPI.
```
    pd->system->removeAllMenuItems();
```

I want to hook in my own things to spy on them.

So I have a place to store the original table:

```
static struct playdate_sys originalSystem;
```

*/
