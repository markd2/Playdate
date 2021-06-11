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
