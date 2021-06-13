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

----------

File system is standard unixy things  No need to dig into them now

----------

Graphics stuff.

Clear color:

    things rendered in kColorClear, those drawn pixels will be set to
    transparent. When you later draw the image into the framebuffer, those
    pixels will not be rendered, i.e., will act as transparent pixels in
    the image

So sounds like maybe bitmaps have an alpha channel, but the framebuffer does not.

----------

drawing functions use a context stack to
  - select drawing target
  - setting a stencil
  - changing draw mode, etc
The stack is unwound at the beginning of each update cycle, drawing restored
to targeting the display.

push/pop context.  Give it a LCDBitmap as the target. (NULL) for using the
display framewbuffer.

stencil - for limiting the area rendered.

lua docs have link to https://en.wikipedia.org/wiki/Stencil_buffer

sprites get stencil images - "to be set on the frame buffre before the srpite is dawn"

Lua's `setInverted` says if it the image is used as a stencil, the behavior is
"black=>drawn, white=>nothing drawn", which implies taht white is what comes
through - sounds similar to the layer mask in photochomp.

In general Lua has much documentation, very word. This is a scan through
the C API before diving in.

draw modes:
  - copy
  - white/black transparent
  - fill white/black
  - XOR / NXOR
  - inverted

There's a clip rect.

line cap style (usual suspects)

also a font - set for subsequent user in drawText calls

text tracking - looks like leading?  spacing between letters?
   the lua font:set/get tracking is talking about spacing between letters
   lua also has leading as well. (spacing between lines)

----------

supporting types

LCDColor - solid color or a pattern
  - color is black/white/clear/XOR
  - pattern is a uint8_t[16]  (not square. interesting)

LCDRect - left/right/top/bottom.  right/bottom not inclusive
PDRect - x/y/width/height
  - huh. two rectangle types

string encoding
  - ascii
  - utf8
  - 16bit LE - Unicode Transformation Format - 16-bit Little Endian

Guessing LCDRect is the lower-level type, PDRect higher-level

----------

Bitmaps

clearBitmap (to a color)
copyBitmap - makes a new copy that's an exact copy
checkMaskCollision
  - two bitmaps, two points, and two flip values
  - returns true if any opaque pixels in bitmap1 (positioned at x/y/flip)
    over lap any opaque pixels in bitmap2 (positioned at x/y/flip), within
    a rectangle.
    - or 0 if none overlap (or the two bitmaps endup non-intersecting)
drawBitmap - draws at x/y/flip
drawScaledBitmap - x/y/xscale/yscale.  no flip, but negative scales do the same thing
freeBitmap - releases resources
getBitmapData - get meatdata (width, height, rowbytes, has a mask, data)
loadBitmap - loads from filesystem
loadIntoBitmap - loads into previously allocated bitmap
newBitmap - new bitmap with width/height/color
tileBitmap - draws anchored at (x,y), tiled inside of a (width, height) rectangle.
transformedBitmap - returns a new rotated/scaled bitmap given rotation (degrees),
  scale.  Returns single int of the allocated size (not sure what that is)

Bitmap tables

getTableBitmap - given a table and an index, get an index
loadBitmapTable - given a path, make a table (I think the filename encodes the
    metrics
loadIntoBitmapTable - load into a previously allocated table
newBitmapTable - makes a new table that can hold X widthxheight bitmaps.

Fonts and text

drawText - text (void*), length, string encoding, at x, y.
  - default is Asheville sans 14 light is the default
getFontGlyph - gets a bitmap for a given character.  Can return advance
  through an optional int pointer.
getFontKeming - returns the keming adjustment between two characters
getTextWidth - given a font, text, length, encoding, and tracking, retursn the 
  width.  (So tracking does seem to be a keming thing)
loadFont - given a path, returns LCDFont.

Gee I'm a Tree

drawEllipse - draw inside of a rectangle (x,y,w,h), with lineWidth.
  Start and end angle (degrees), clockwise from due north.  If
  start/end angle are equal, draws whole ellipse
fillEllipse - same, but also has color, but no line width
drawLine - (x1y1 x2y2, width, color)
drawRect - (x,y,w,h, color)
fillRect - same
fillTriangle - x1y1 x2y2 x3y3 and color


misc

clear - fill display with color
setBackgroundColor - what's shown in the display when it's offset, and for
    clearing dirty areas in the sprite system
display - flushes the frame buffer to the display.
    called automatically after each runloop spin, "so shouldn't be any need to
    call it yourself"  (now I want to find one...)
getDebugBitmap - just in simulator - returns the debug framebuffer as a bitmap
    (what is the debug bitmap?)
    lua has a `playdateDebugDraw
    lua has debugDraw to highlight regions on the simulator screen in a different color
    Guessing I do stuff to this and have it appear on the simulator?
getDisplayFrame - returns copy of the last completed frame as a bitmap
getFrame - returns the current display frame buffer.
   rows are 32-bit aligned, so the row stride is 52 bytes, with the extra two
   bytes ignored (wonder if they stick around).  MSB-ordered.
   So pixel in column 0 is the 0x80 bit of the first byte of the row
getFrameBufferBitmap - returns a copy of the contenst of the working framebuffer
   as a bitmap
getanguage - current language of the system (english / japanese / unknown)
markUpdatedRows - given start/end row (inclusive).  A way to tell the graphics system
   that you've directly touched the pixel in the rows.  0,LCD_ROWS-1 says to
   update the entire display.
setDrawOffset - (dx,dy) offsets the origin for drawing calls (can be negative)
   suggested use - centering a camera on a sprite that is moving around a world
   larger than the screen
setSpriteDrawFunction - installs a native drawing function for the given (lua) sprite
   the function takes sprite / bounds / frame / drawRect
     - bounds is the current bounds of the sprite, drawRect is the are to draw, frame
      is the raw framebuffer (saving a call to getFrame)
setColorToPattern - sets a color to an 8x8 pattern using the given bitmap.  x,y is the topleft
   of the pattern

Video

loadVideo  - opens pdv file and returns a new video player
freePlayer - releases resources
setContext - sets the rendering destination to the given bitmap.  If it fails,
   returns 0, and can use getError() for the error on the player
useScreenContext - set the destination to the screen
renderFrame - render frame N into the current context. (0-return, error)
getError - returns text describing most recent error

Input

setPeripheralsEnabled - crank and accel are disabled to save power.  The mask
   is none / accelerometer / crank.  (not impmeneted, so assuming the crank is always
   powered)
getAcceleromter - the last-read accel/magneto data (float &).  unimpl, so dunno what the
   returned values are.
getButtonState - already figured this out
getCrankAngle / getCrankChange - either the angle (0-360), or delta (negative are counter-clockwise)
isCrankDocked - 1 or 0

setAutoLockDisabled - autolocks with 60 seconds of no input
setCrankSoundsDisabled - turns off sound for system events - in particular crank [un]dock.
    this mutes the default if you want to do your own. Returns the previous value.


JSON stuff
  - ignoring

Lua stuff
  - can add functions or tables
  - ignoring
  - can call lua from C  (pd->lua->callFunction)
    "this is slow, so use sparingly"

Sprites!

memory stuff

newSprite - makes a new sprite
copy - allocates and returns a copy
free - releases resources

properties

setBounds (LCDSprite, but takes a PDRect)
getBounds - gets the bounds
moveTo - move to x,y (float !) and resets its bounds based on bitmap dimensions and center
moveBy - move via dx,dy
getPosition - get the position of sprit (int)
setImage - set the image to the given bitmap (+ flip)
getImage - gets the LCDBit map assigned to the sprit
setSize - float width/height - used to set the bounds when calling moveTo
setZIndex - higher Z sprits drawn on top
getZIndex - fetcher
setTag - uint8_t tag - useful for identying sprites / types of sprites when using
    the collision API
getTag - gets the tag
setDrawMode - the drawMode
setImageFlip - flips (the four-way)
setStencil - stencil to be set on the frame buffer before the sprite is drawn
setClipRext - sets clip rect for the sprite drawing (not sure that actually means)
clearClipRect
setClipRectsInRange - given a rect, startZ and endZ (inclusive). - sets clipping rectangle
   for _all_ sprites with z indexes in that range  !!! one of the oddly specific ones
   that's probably extremely cool for something
clearClipRectsInRange - startz / end - clears clip rect for all sprites in that range
setUpdatesEnabled - flag - determine whether the sprite has its update function
   called.
updatesEnabled - fetch
set/isVisible - if it's visible
setOpaque - tells the system it doesn't need to draw anything underneath the sprite
   since it'll be overdrawn anyway.  If set an image w/o mask/alpah on the sprite,
   it automatically sets the opaque
setAlwaysRedraw - it will always redraw
addDirtyRect - marks the given rect as needing a redraw
setIgnoresDrawOffset - it will always draw in screen coordiantes, ignoring the 
   current drawOffset.  Only affects drawing, so don't use it for collisions that
   happen in world-space
setUpdateFunction - sets the update function (that takes a sprite)
   used in Sprite\Game for things like 'update enemy plane', which does things
   like movTo, or remove/free when it gets off screen.
setDrawFunction - sprite / bounds / frame / drawRect like above.

(sprite) display list

add/removeSprite - adds to the display list so it's drawn in the current scene
removeSprites - removes array of sprites
removeAllSprites - blor
getSpriteCount - blorf
drawSprites - draws every sprite in display list
updateAndDrawSprites - updates and draws every sprite in the list.

collisiosn

the caller is responsible for freeing any arrays returned by these
resetCollisionWorld - frees and reallocates internal collision data, resetting to
   its default state
setCollisionsEnabled - sets the collisionsEnabled flag on the sprite (along with the
  collide rect) .  on by default
collisionsEnabled - query
setCollidateRect - marks the area of the sprite relative to its bonds, to be checked
   for collisions
getCollideRect - query
clearCollideRect - clears it
setCollisionResponseFunction - sets the filterProc on the sprite
  - the function returns a response type (see below)
    - slide
    - freeze
    - overlap
    - bounce - there's docs in lua-land
  - is given two sprites.
checkCollisions - takes sprite, goalX/Y (float), actualX/Y pointers (float), length pointer
  - returns same values as moveWithCollisions vvv but does not move the sprite
moveWithCollisions - sprite, goalX/Y, actualX/Y pointers, length pointer.
  - moves the given sprite towards goalX/goalY taking collisions into account
  and returns an array of SpriteCollisionInfo (it's hefty. See below)
  len is set to the size of the rray and actualX/Y are set to the sprite's position
  after collisions (len for the size of the collisionInfo array)
  The actualx/y is set to the sprite's position after collisions.  If no collisions,
  same as goalX/Y
  - the info is a struct, so grab a poitner with &results[i] (to avoid copying
    overhead)

Sprite Collision Info(e)

```
struct SpriteCollisionInfo {
	LCDSprite *sprite;
	LCDSprite *other;
	SpriteCollisionResponseType responseType;
	uint8_t overlaps;
	float ti;
	CollisionPoint move;
	CollisionVector normal;
	CollisionPoint touch;
	PDRect spriteRect;
	PDRect otherRect;
};
```

- type of collision (slide, etc), overlaps - 1 if sprite was overlapping when the
collision sstarted, zero if it didn't overlap but tunneled through other
- ti - 0...1 - how far along the movement to the goal the collision occured
- move - difference between original coordiantes and the actual ones when the
  collision happened
- normal - collision normal - usually -1, 0, or 1 in x and y.  Use to determine
  things like if your character is touching the ground
- touch - coordinates where the sprites started touching each other
- sprite rect, rect the sprot occupied when touch happen
- otherRect - rect other occupied when touch happened

collision point is x,y float
collision vetor is x,y int;

response types
  - slide - use for collisiosn that should slide over other objects, like
    SuperMario does over a patform or the groun
  - freeze - where sprite should stop moving, like an arrow hitting a wall
  - overlap - used for collions where want to know about the collision, but
    it shold not impact the movement, like when collecting a coin
  - bounce - use when sprite should move way from each other ,like ball in Pong

interesting memory allocation happening for each collision check

queryQpritesAtPoint(x, y, *len) - returns array of all sprites with collision
  rects containing the point x,y.
querySpritesInRect(x, y, width, height, int *len) - array of all sprites with
   collision rects that intersect the rect.
  query sprites along line (x/y1, x/y2, *len) - array of all sprites with
  collision rects that interset the line connecting the two points
querySpriteInfoAlongLine (x/y1, x/y2, *len) - returns array of SpriteQueryInfo for
all sprites with collisions rects that intersect the line.  The above call is 
faster if you don't need all the info(e)

```
struct SpriteQueryInfo
{
    LCDSprite *sprite;
    float ti1;
    float ti2;
    CollisionPoint entryPoint;
    CollisionPoint exitPoint;
};
```

sprite is what's intersected by teh segment
ti1 and ti2 - 0...1, how far from the starting point of the line segment
the collision happened.  ti1 from entry, ti2 from exit
entry/exit - coordinates of the first/second intersecton between
sprite and line sigment

overlappingSprites - give a sprite, *len - array of sprites that have
   collide rects that are overlapping the given sprites collide rect
allOverlappingSprites - all sprites that have collide rects that are currently
   overlapping.  Ech consecutive pair is overlapp (0&1 overlap, 2 & 3 overlap)



----------

idea - use the menu options for different experiments.  Some kind of 
polymorphism (or callback) for "installing" what is getting shown.
So can have :alot: of experiments that can choose from the menu.

pong / breakout  game using bounce collision