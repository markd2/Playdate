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

* to tell if you're on a simulator or not

```
int IsSimulator(void) {
    // device has long size of 4
    return sizeof(long) == 8;
}
```
 
### Building

```
% cd directory-that-contains-source
% ls
source/         art/
% pdc source game.pdx
% open game.pdx
```

### Run on device

```
% open game.pdx
(simulator) command-L (put up sim lock screen)
(simulator) command-U (run on device)
```

### Random

Caveman debugging in C

```
pd->system->logToConsole("format string %d", 23);
```

Debugging with lldb (simulator only)

```
lldb ~/Developer/PlaydateSDK/bin/Playdate\ Simulator.app/Contents/MacOS/Playdate\ Simulator
```

Location of stdlib headers
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

Handles varargs!

```
#define debuglog(...) _debuglog(__FILE__, __LINE__, __VA_ARGS__)
void _debuglog(const char *file, int line, const char *format, ...) {
    char buffer[1024];

    va_list argList;
    va_start(argList, format);
    vsnprintf(buffer, 1024, format, argList);
    va_end(argList);
    
    pd->system->logToConsole("%s:%d %s", file, line, buffer);
} // debuglog
```

----------

Looks like memory allocations have the first bytes zeroed
- Not documented, though.
- Simulator, most of the allocated blocks I get have zeros for the first
  8 bytes.
- on-device often get non-zero values in the first bytes.

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
BUT the pd struct is memory protected, so get crashes when try to
hijack the vtable.  oh well.

----------

time and time.  can get ms from some arbitrary and seconds/ms from an epoch.

```
pd->system->getCurrentTimeMilliseconds());
unsigned int sec, ms;
sec = pd->system->getSecondsSinceEpoch(&ms);
```

some timing macros in `timing.h`

```
#define TIMING_START do { \
    pd->system->resetElapsedTime(); \
    float $then = pd->system->getElapsedTime(); \


#define TIMING_END \
    float $now = pd->system->getElapsedTime(); \
    print("elapsed time \(%f)", (double)($now - $then)); \
} while(0);
```

A 100K timing loop

```
        for (volatile int i = 0; i < 100000; i++) {
            (void)i;
        }
```

is 0.000440, 0.000755 on an m1 Mini, an .006000 on a playdate.

Adding a value to a float/double in that loop (upped to ten million loops).

```
    double blah = 3.1415;
    volatile double constant = 0.1234;

    print("sizeof %d vs %d", sizeof(float), sizeof(double));

    TIMING_START {
        for (volatile int i = 0; i < 10000000; i++) {
            blah += constant;
        }
    } TIMING_END
```

that's .556 seconds (double), 0.493 (float).  Not a huge 
difference (12%?).  Even though the docs say double is "extremely slow",
so probably something wrong with the methodology.

----------


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
values are between 0 and 3, inclusive.".  ~So should cycle back to this
when getting more stuff on the screen.~  Looks like it applies a 
chunkieness factor.  Text looks like bad 1984 mac text scaling.

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
  "tracking value is the number of pixels of whitespace between each character drawn in a string"

```
    int tracking = 10;
    int width = pd->graphics->getTextWidth(font, text, textlen, kASCIIEncoding, tracking);
    width -= tracking;  // in case like drawing into a bitmap

        pd->graphics->setTextTracking(tracking);
        pd->graphics->drawText(text, textlen, kASCIIEncoding, 0, 0);
```

----------

supporting types

LCDColor - solid color or a pattern
  - color is black/white/clear/XOR
  - pattern is a uint8_t[16]  ~(not square. interesting)~  8 rows pattern, 8 rows mask.

Ooh, and there's binary literals.

```
static LCDPattern fillPattern = {
    // pattern
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,

    // mask
    0b11110000,
    0b11110000,
    0b11110000,
    0b11110000,
    0b00001111,
    0b00001111,
    0b00001111,
    0b00001111,
};
```

and assign

```
LCDColor color = (uintptr_t)fillPattern;
```

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

----------

Fonts and text

drawText - text (void*), length, string encoding, at x, y.
  - default is Asheville sans 14 light is the default
  - return value is the width of the text
  - INTERESTING - if there's a newline, it automatically advances

getFontGlyph - gets a bitmap for a given character.  Can return advance
  through an optional int pointer.
getFontKeming - returns the keming adjustment between two characters
getTextWidth - given a font, text, length, encoding, and tracking, returns the 
  width.  (So tracking does seem to be a keming thing)
loadFont - given a path, returns LCDFont.

font file format in the Lua docs - https://sdk.play.date/inside-playdate/#C-graphics.font
  - .fnt file - plain text
  - `space` for the width of a space
  - pairs of glyph and width (in the order, L->R, T->B) in the file a
  - keming pairs supported
  - can embed the font's pixel data by using datalen, base68, and pixel cell 
    width and height

Lua-land lets you specify a set of fonts for normal / bold / italic 

----------

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
    (white shows through, black blocks)
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

Audio

getCurrentTime - (uint32_t) sound engine's current time value, in units of
   sample frames.  32 bit int.  that's like 27 hours?  what happens if wraps
   around?

a sound channel contains sound sources and sound effects (very opaque structure)

getDefaultChannel - returns default channel - where sound sources play if they
    haven't been explicitly assigned to a different channel
addChannel - adds the given channel to the sound engine
removeChannel
addSource - adds callback function that'll be called on every audio render cycle.
  The headers have "_deprecated" for the callbsck
setMicCallback(audioInputFunction, userdata, forceInternal(int))
  - the callback will be called with the recorded audio data, monophonic stream
    of samples.  Return 1 to keep recording, 0 to stop.
  - if forceInternal is set, the device mic is used whether the the headset has a mic
getHeadphoneState(int *headphone, int *mic, changeCallback(int headphone, int mic))
  - sets headphone (if not NULL) to 1 if headphones currently plugged in
  - mic is set if the headphones include a microphone
  - if changeclalback is provided, will be called when the status changes.
    - and audio output will *not* automatically switch - the callback should use
       sound->setOutputsActive
setOutputsActive - force audio output to the given outputs (headphone / speaker)
   regardless of the headphone status

channels

newChannel/freeChannel - makes a new sound channel object
addSource/removeSource - adds or removes a _soundSource_ to the channel.
  If a source is not assigned to the channel, it plays on the default global
  channel.  Removing returns 1 if the source was found (and removed from) the channel

addEffect/remove - on the tin

get/setVolume - float volume [0-1]

get/setVolumeModerator - channel / PDSynthSignalValue*mod
  - sets/gets a signal to modulate the channel volume
    PDSynthEnvelope and PDSynthLFO are subclasses of PDSynthSignal

get/setPan - float pan. [-1, 1] where -1 is left, 0 center, 1 right
get/setPanModulator - takes a PDSynthSignalValue
addCallbackSource - creates a new 
  - takes a channel, audioSource function, context, stereo flag
  - the callback takes a context, int16_t *left / right, length
    - no right if it's a stereo source.
  - the function should fill in the buffers.  return 1 to play, or 0 if the source
    is silent through the cycle.
  - caller takes ownership of the allocated source, and should free it
    (I don't understsand why it's talking about memory management here)
getDryLevelSignal - returns a signal that follows the volume of the channel before
   effects are applied
getWetLevelSignal - returns a signal that follows the volume of the channel after effects
   are applied.

soundSource is the parent class of FilePlayer, SamplePlayer, PDSynth, and 
DelayLineTap.  Any objects of those types can be cast to a soundSource and used here.

set/getVolume - set left/right volume. L/R returned via pointer   [0, 1.0]

isPlaying - 1 if the source is currently playing

setCompletionCallback - called when source has finished playing

AudioSample

newSampleBuffer - given a length, returns a new AudioSample with a buffer large enough
  to load  file of given bytes
loadIntoSample - load sound data from file into an existing AudioSample
loadFile - allocates and returns a new AudioSample with the sound data from a given file
newSampleFromData - given a uint8_t buffer, sound format, sample rate, and byte count,
  will return a new AudioSample referencing the audio data.  It references the pointer
  rather than copying. So keep the buffer around
getData - retrieves the sample data (unclear if we allocate the buffer or not - guessing
  not given the newSampleFromData behavior)
freeSample - frees the AudioSample
getLength - returns length in seconds (float) of the sample

FilePlayer

newPlayer / freePlayer - on the tin
loadIntoPlayer - given a path, prepares a player to stream the file from there
pause - pauses the file player
play - plays a given FIlePlayer. repeat is > 1, it loops the given number of times.
    If zero, loops endlessly until stopped
isPlaying - returns 1 if olaying, zero if not
setBufferLngth - sets buffer length to bufferLen seconds
getLength - returns length in seconds of the file loaded into the player
setFinishCallback - to be called when playback has completed
  - an alias for setFinishCallback()
didUnderrun - returns 1 if the player has underrun
  - that is, the buffer is draining faster than being filled
setLoopRange - player, start, end (both floats) - in seconds.  
   - if end is omitted (?), the end of the file is used.  (I guess two methods?)
   - nope. Dunno how it's going to do that. Guess pass it zero?
setOffset - set current offset in seconds (float)
set/getRate - playback rate - 1.0 is normal, 0.5 down an octave, 2.0 up an octave, etc.
setStopOnUnderrun - if true, player will restart (after a stutter) as soon as data
    is available. (kind of a strange way to name the method)
set/getVolume - left/right volume (float) for channels of the player
stop - stops playing the file
fadeVolume - player, left, right, int32_len, finishCallback
  - changes the volume of fileplayer to left/right over a lenght of len sample frames,
    then calls the provided callback if given

Sample Player

getLength - length in seconds of sample in the player
isPlaying - 1 if playing, zero if not
newSamplePlayer - makes a new empty saple player
play - repeat / rate.  Like above.  If repeat is -1, it ping-pong loops
setFinishCallback - called when playbacck has completed
set/getOffset - sets offset in seconds
set playrange - start, end in frames
setPaused - pauses/resume
set/getRate - as above
setSample - assigns AudioSample to the player
set/getVoume - set playback volume of left/right channels
stop - stops
freePlayer - frees

PD Synth

new/free Synth - new syntho object
setWaveform - one of these
   square, triangle, sine, noise, sawtooth
   POPhase, PODigital, POVosim (voice simulator)
setGenerator - provides a custom waveform generator function.
  (not planning on this any time soon, so...)
setSample - provides a sample for the synth to play - uncompressed PCM (not ADPCM)
setAttack/Decay/Sustain/Release - float values for ADSR
setTranspose - transposes output by given number of half-steps (float).
  - wonder if can do gliss by changing the transpose
set/getFrequencyModulator - synth and synthSignalValue
  - sets/gets a signal to modulate the frequency.  Signal is scaled so a value of
    1 doubles the synth pitch (octave) and -1 halves it
set/getAmplitudeModulator
  - same, but for the amplitude
getParameterCount - returns the number of parameters advertised by the synth
  - docs called this setParameter)
setParameter - takes paramter number and value.  Returns 0 if num is not valid
set/getParamterModulator - set a signal for a paramter
  - (these docs are rough with copy and paste errors)
playNote - synth, frequency, velocity, length, when (uint32)
playMidiNote - synth, MIDINote, velocity, len, when
  - play a note on the synth with frequncy or MII note (C4 = 60)
  - len -1 to leave note playing until a note off.
  - if when is 0, note is played imediately, otherwise scheduled for the given time
    (use sound->getCurrentTime() to get the curren time)
noteOff (synth)
set/getVolume - (l/r)
isPlaying - is the synth currently playing

PDSynthInstrument

collects a number of PDSynth objects to provide polyphony.  "bank of voices for
playing a sequence track"

new/freeInstrument - makes a new instrument.  free removes from the sound engine if needed
addVoice - instrument, PDSynth, MIDINote rangeStart/End, transpose
   - will response to playNotes between start and end (inclusive). Transpose is half-step
     units, added to the instrument's transpose parameter
playNote/MIDI Note - passes the play(Midi)note to the synth in the collection that
has been off for the longest, or has been playing longest if all synths are currently playing
   - the synth that recived the play note is returned
noteOff - forwards note of to the synth currently playing the note.
pitchBend/Range - given a float end, or float halfSteps. Applied to the voices in the
 instrument
setTranspose - sets for all voices
allNotesOff - sends noteOff to all voices
set/getVolume - left/right
activeVoiceCount - number of voices currently playing

----------

Signals

represents a signal that can be used as a modulator.
PDSynthSignal is used for 'active' signals that change their values automatically,
like LFO and Envelope

LFO

newLFO(type)
  - square, triangoe, sine, sampleAndHold, sawtoothUp/Down, Arpeggiator, Function
free - frees
setType - sets the LFO shape to the type
setRate - cycles per second
set phase - sets LFO phase
setCenter/Depth - sets the center or depth of the LFO
setArpeggiation - given values are in half-steps from the center note.
  Give it an array of steps - 0,4,7,12 plays the notes of a major chord
setFunction - custom function for LFO values
  - function takes lfo, userdata, returns a float.
setDelay - sets initial holdoff time for the LFO where the LFO remains at its
   center value, and a ramp time where the value increases lineariluy to its max
   depth.  float in seconds
setRetrigger - if on, the LFO's phase is reset to 0 when a synth starts playing a note
getValue - gets the current output value of the LFO

Envelope
newEnvlope - new PDSynthEnvlope with ADSR
free - free
setAttack/D/S/R - on the tin
setLegato - if set, when the envelope is retriggered before it's released, it
  remains in teh sustain phase instead of boinging back to the attack phase
setRetrigger - if on, the envelope always starts at 0 when a note starts playing,
  instead of the current value if it's active
getValue  -return the current value


Sound Effect

parent class of the sound effect types
TwoPoleFilter, OnePoleFilter, BitCrusher, RingModulator, Overdrive, and DelayLine

newEffect(effectProc, userdata)
  - new effect using the given processing function.
  - proc is effect, int32_t *left/*right, nsSamples, bufactive flag
  - bufactive is 1 if the samples have set in the left or right buffers
  - functions should return 1 if it changed teh buffer samples, otherwise 0.
  - left and right are Q8.24 format
freeEffect - frees
setMix - float level. set wet/dry mix for the effect.  1 is full wet, 0 leaves
    the effect out of mix
    - "useful if you're using a delay line with taps and don't want to hear the delay
       line itself"
set/getMixModulator - set a PDSynthSignal to modulate the effects mix level
set/getUserdata - on the tin


TwoPoleFilter
effect->twoPoleFilter->newFilter - makes a new effect
freeFilter - on the tin
setType - lowPass, highpass, BandPass, notch, PEQ, low shelf, high shelf
    (parametric EQ)  (holy cow)
setFrequency - set the center/corner frequency in Hz
set/getFrequencyModulator - give a synth signalnto modulate the frequency
  - signal is scaled so a value of 1.0 corresponds to half the sample rate
setGain - sets the filter gain
setResonance - sets the filter resonance
setResonanceModulator - you know how this goes

OnePoleFilter

simple low/highpass filter, with a single cutoff frequency

newFilter/free - on the tin
setParameter - set the single parameter, the cutoff.
      talking about 0-1 -1-0 being high/low pass, but then how to do 
     frequency?
setParameterModulator - as above


BitCrusher

newBitCrusher/free
setAmount - float. amount of crushing.  0 (no effect) to 1 (quantizing output to 1bit)
setAmountModulator - set a signal to the crushing amount
setUndersampleModulator - sets a signla to module the undersampling amount
  - not sure what undersampler is.  The "amazingnoises" grain mangler effect says
    "1 - sample freqeuncy is not changed. with 0.5 it's halved - 44100 downsampled
     to 22050.

Ring Modulator

newRingmod/free - make/free
setFrequency - float frequncy of the modulation signal
set/getFrequnecyModulator

overdrive

newOverdrive/freeOverdrive - on the tin
setGain - float, gain of the overdrive effect
setLimit - level where the amplified input clips (float)
setLimitModulator - use a signal
setOffset - adds an offset ot the upper and lower limits to create an asymmeric clipping
setOffsetModlator - use a signal

DelayLine

new/free - blahblahblah
setLength - change the length of the delay line, clearing its contents
  - int frames
setFeedback - float - feedback level of the delay line
addTap - returns a new tap on the delay line at the given delay position (int),
  delay <= the length of the delay line

Delay Line Tap

this is a Sound Source, not an effect. A delay line tap can be added
to any channel, not only the channel the delay line is on.

free a tap - on the tin
setTapDelay - frames
  - sets the position of the tap on the delay line, up to the delay line's length
setTapDelayModulator - add a signal
  - if it's continuous (envlope or triangle LFO, but not square LFO), playback
    is sped up or slowed down to compress or expand time.  (WHOA)
setTapChannelsFlipped - if the delay line is stereo and flip is set, the tap
  outputs the dlay line's left channel to its right output and vice versa

SoundSequence

new/freeSequence
loadMIDIFile - sequence and a path.  1 on success, 0 on failure

get/setTime - gets/sets the current time in the squence.  in samples since
    the start of the file.  The step this moves the sequence to depends on the
    tempo

setLoops - looping range (startstep / endstep, int loops).  If loops is 0, it
    repeats

getTrackCount - number of tracks
addTrack - takes soundSequence,r eturns a SequenceTrack
get/setTrackAtIndex - on the tin
allNotesOff - stop!


SequenceTrack

A SequenceTrack comprises a PDSynthInstrument, a sequence of notes to
play on that instrument, and any number of ControlSignal objects to control
parameter changes.

new/freeTrack
setInstrument / getInstrument - associates a PDSynthInstrument to a squence track
add/removeNoteEvent - adds single note to track (uint32_t step, MIDINote note)
clearNotes - clear notes from track
getControlSignalCount - number of control objects on teh track
getControlSignal - get control signal at index
clearControlEvents - clears all control signals from teh track
activeVoiceCount - number of voices currently playing in the track's instrument
getPolyphony - returns the max number of simultanously palying notes
  (value is onluy set whe nthe track loaded from a midi file.  Don't track
  for user-created events)
setMuted - mute/unmute track.


Control Signal

Subclass of PDSynthSignal used for sequencing changes to parameters.
(automation)

newSignal / freeSignal - make/nuke
clearEvents - clears all events from the given signal
addEvent - given a step, value, and interpolate.  Adds a value to the
      signal's timeline at the given step. If interpolate is set, the
      value is interpolated between the previous step + value and this
      one.  
removeEvent - removes the control event at the given step
getMIDIControllerNumber - returns the midi controller nubmer for this
   control signal, if it was created from a MIDI file


----------

And that's it.  (outside of json and lua gaskets).  That's all you need
to know.

----------

ok, so an experimental system with things in the menus.

In An OO world, it'd be something like this.

These are the independent units (objects)
+---+   +---+
|   |   |   |
+---+   +---+

There's a menu that publishes them (so maybe category plus name)
like Synth - filters / Synth - MIDI  Drawing - lineCaps  Darwing - font  Sprites - game.

So the main.c will set up stuff.  Populate the menu.  The event handler will shunt
events to the object.  Its own buttonPumper.  Its own updateCallback.

So, properties and whatnot
  name
  category
  updateCallback
  suspend
  resume

each can make its own pumper, since that's driven by the update callback

----------

Also needing to figure out how to do a library kind of thing.  (like memory)

oh bummer, a new version THAT I CAN'T GET :'-(

----------

in doing the Sampler, getting crashes on reload. Took a tour into hooking
into kEventTerminate to clean up, but I'm guessing there's no need to - the OS
will just wipe the stack/heap clean, reset all OS services, and launch the new game.

The `pd` substrucutres (like `system`) all have their pointers clearned by that point

```
(lldb) print *pd->system
print *pd->system
(const playdate_sys) $5 = {
  realloc = 0x0000000000000000
  formatString = 0x0000000000000000
  logToConsole = 0x0000000000000000
```

So trying to do `free` (via realloc) through that will ... not work.

hrm, even on the reinit, all that stuff in null.

really not understanding it.  Do the existing C samples reload twice?
(c-fun did reload).  And SpriteGame reloads.

OK, so why is this stuff dying

LUCKILY doing it to myself.  uninstallSpies. need to observe the installed flag.

----------

Looks like no drawing support for angles.

There is LCDBitmap transformedBitmap that should be able to do that?

----------

Custom fonts!

The SDK has some .fnt files.  Copy those into $PROJECT/Source/font, e.g. 
e.g. font-Bitmore-Medieval-Outlined.fnt

but getting this error

error: font/font-Bitmore-Medieval-Outlined.fnt should either have matching image table png/gif or encoded image data in the file

ah, need the png as well

./Resources/Fonts/Bitmore/font-Bitmore-Medieval-Outlined-table-9-13.png

So copying those to Source/font, load with

```
    const char *errorText;
    font = pd->graphics->loadFont("font/font-Bitmore-Medieval-Outlined.pft", &errorText);
    if (errorText != NULL) {
        print("couldn't make font %s", errorText);
    }
```

----------

MacPaint - for 8x8 patterns

https://archive.org/details/mac_Paint_2

----------

Draw mode is just for images (and fonts, which are images)

stencil - white pixels show, black pixels block (like layer mask)

----------

bitmap things to play with

X - load from disk
X - getting bitmap data (data pointer)
X - bitmap flip
X - drawing scaled bitmap
X - check mask collision
X- transform bitmap
  - has rotation
  > maybe button+dpad to change scale
  - that'll give some more sophisticated combos and see how the pumper works
  - maybe put into another sample, transform / scaling / collisions.
X draw mode
  - maybe another option menu for the draw mode
  - or maybe another-button+dpad
  - asking the sample for a menu
    - make sure draw modes work for those images
  - or maybe the crank
X maybe nothing moves, but dpad tweedles different scenarios for a bitmap being drawn
  on top and underneath various patterns.
X drawing on black and white
X stencil



----------

idea - use the menu options for different experiments.  Some kind of 
polymorphism (or callback) for "installing" what is getting shown.
So can have :alot: of experiments that can choose from the menu.

pong / breakout  game using bounce collision. crank to move paddle.

and omg the synth stuff.  that is CRAZY.

Rehearsal Buddy - if can do FFT on the mic, have a tuner.  The crank would be cool
for changing a metronome. Use the synths to make sounds that can cut through various
ensembles.

[X] something to play with blendmodes

something to play with just updating affected rows (c.f. gingerbeard in the discord june 28)

----------

huh, does the simulator not play audio?  My audio file isn't playing there, but
does on the device (and sounds amazingly good)