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
