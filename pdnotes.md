# Playdate Notes

## Important specs

* 400x240 screen, 1 bit
* 30 fps by default, max 50 fps
* d-pad (8-way), two buttons, menu button, lock button, crank, accelerometer
* internal speaker / mic / headphone jack with mic input
* 16 meg RAM, ~2~ 4 GB storage
* keep games no bigger than 20-40meg

### Tools n'at

* `pdc` - compiles Lua source files / art / sounds into a pdx bundle
* `pdutil` - sednd text commands to the device firmware.
* `Playdate Simulator` - app that runs pdx files. Can also run from
  command line by digging into Contents/MacOS
* pdxinfo - metadata
   - launcher card is 350x155
   - also menu cards, launch images, etc
 
### Building

```
% cd directory-that-contains-source
% ls
source/         art/
% pdc source game.pdx
% open game.pdx
```

### Random

Caveman debugging in C

```
pd->system->logToConsole("format string %d", 23);
```

Debugging with lldb

```
lldb ~/Developer/PlaydateSDK/bin/Playdate\ Simulator.app/Contents/MacOS/Playdate\ Simulator
```

Location of headers
```
/usr/local/playdate/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/include/stdlib.h
```


### Other Stuffs

C entry point (like main)

```
int eventHandler(PlaydateAPI* playdate, 
                 PDSystemEvent event, 
                 uint32_t arg) {
```

with lifecycle enums.


----------

Trying to use vsnprintf causes a link error . Just vsprintf too.

```
function `_sbrk_r':
sbrkr.c:(.text._sbrk_r+0xc): undefined reference to `_sbrk'
```

OOH NICE 

```
int playdate->system->formatString(char **ret, const char *format, ...)
```
we're responsible for freeing `ret`



oh well.  maybe when get a device see if it's just a weird intel linker thing.

```
#define debuglog(...) _debuglog(__FILE__, __LINE__, __VA_ARGS__)
void _debuglog(const char *file, int line, const char *format, ...) {
    char buffer[1024];

    va_list argList;
    va_start(argList, format);
    vsprintf(buffer, format, argList);
    va_end(argList);
    
    pd->system->logToConsole("%s:%d %s", file, line, buffer);
} // debuglog
```

----------

Looks like memory allocations are automatically zeroed.
Not documented, though.

----------

Doing a basic ADT-style API (see buttonpumper).  Toyed with doing a
Galaxy #define vtable stuff, but since there's nothing polymorhic (yet),
no need to go to the trouble.

----------

System menu (continuing to work down through utility functions)

* can add up to three items to the system menu
  - normal
    * When invoked, a menu teim will invoke the callback, hide the menu, unpause game
      and call eventHandler() with kEventResume
  - checkmark
    * 0 for unchecked, 1 for checked
    * when interacted with, the callback is called when the menu is closed
  - options
    * allow user to cycle through a set of options
      - array of strings.  Keep short because of space issues
    * when interacfted with, the callback is called when the menu is closed.

There's add calls for all three.  Remvoe a particular menu item. Remove all.
Getters/setters for the menu item's title, integer value

There's also setting a menu image. 400x240 LCDBitmap.  Put important content on left-side
of the image in 200 pixel width (menu will obscure the rest). Right side will be visible
as menu animates
Can give a non-zero offset
"this could be called in response to the kEventPause" in the event handler

So, sounds like we can have a menu manager.  Items with callbacks.
Try playing first, though.

??? seem to always be getting nil in the menu item callback

using a unicode chracter (struck-O) has a <?> placeholder in the name.

The options is a `TITLE   current option` and using arrows or pressing B cycles through
them.

Looks like all text is bashed to lowercase

I don't know the UI context for all that stuff to want to put a wrapper around it yet.
Maybe a chance for vgalaxyPolymorhpism.

----------

added spy for `system` stuff.  Tedious, but do-able.  yay macros.

----------

time and time.  can get ms from some arbitrary and seconds/ms from an epoch.

```
pd->system->getCurrentTimeMilliseconds());
unsigned int sec, ms;
sec = pd->system->getSecondsSinceEpoch(&ms);
```

----------

can sprintf with , but awkward.  And couldn't get a wrapper around it (no va_ version),
and couldn't figure out __builtin_apply/_args/_return
c.f. https://stackoverflow.com/a/61545790

```
        char *formatted;
        pd->system->formatString(&formatted, "hello %s", "sailor");
        // use it
        pdFree(formatted);
```

--------------------

last misc function is setMenuImage.  Will need to figure out how to make an LCD Bitmap
400x240
left-half useful stuff 200px wide

so, how to make an LCDBitmap?

LCDBitmap* playdate->graphics->loadBitmap(const char* path, const char** outerr);

now need a path %-)

        pd->system->formatString(&path, "images/explosion/%d", i+1);

(and the SpriteGame sample code doesn't free it %-) )

`Source` has images/explosion/[1-8].png.


So something like this:
```
    const char *path = "images/menuImage";
    const char *error;

    LCDBitmap *bitmap = pd->graphics->loadBitmap(path, &error);

    if (error != NULL) {
        print("error is ", error);
    } 

    if (bitmap != NULL) {
        pd->system->setMenuImage(bitmap, 20);
    }
```

Errors are of the form
    error is 'file not found: images/menuImagex'

interesting, image loaded, but got an error of
    error is 'updateWillStart'
so no idea what that is.


----------

Skipping audio for now - it's really deep.

Display!

setInverted doesn't invert the menu image

```
        pd->display->setFlipped(x, y);      // bools I assume
        pd->display->setInverted(inverted); // bool I assume
```

Not sure what mosaic does.  Setting it seems to make the bouncing text
disappear.

From the lua docs, "Adds a mosaic effect to the display. Valid x and y
values are between 0 and 3, inclusive.".  So should cycle back to this
when getting more stuff on the screen

Also a set refresh rate

Scaling embiggens the top-left corner of the framebuffer.  1, 2, 4, 8 are valid values.



