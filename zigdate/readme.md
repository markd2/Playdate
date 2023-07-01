# Zigdate

Programming the Playdate with Zig™, based on https://github.com/DanB91/Zig-Playdate-Template

* using zig-11 - 0.11.0-dev.3777+64f0059cd
* brew install binutils
  - _binutils is keg-only, which means it was not symlinked into /opt/homebrew, because it shadows the host toolchain._
  - add to path: /opt/homebrew/Cellar/binutils/2.40/bin
* PLAYDATE_SDK_PATH set to ~/Developer/PlaydateSDK
  - `cat $PLAYDATE_SDK_PATH/VERSION.txt`
  - `2.0.0`

* `zig build run`

error on `@enumToInt`
  - `playdate.graphics.clear(@enumToInt(pdapi.LCDSolidColor.ColorWhite));`

```
zig build-lib pdex Debug native: error: the following command failed with 1 compilation errors:
/Users/markd/Projects/thirdparty/zig/zig-11/zig build-lib /Users/markd/Projects/RandomLearning/zig/zigdate/Zig-Playdate-Template/src/main.zig --cache-dir /Users/markd/Projects/RandomLearning/zig/zigdate/Zig-Playdate-Template/zig-cache --global-cache-dir /Users/markd/.cache/zig --name pdex -dynamic -install_name @rpath/libpdex.dylib --listen=- 
Build Summary: 0/9 steps succeeded; 1 failed (disable with --summary none)
install transitive failure
+- run /Users/markd/Developer/PlaydateSDK/bin/pdc transitive failure
   +- run objcopy transitive failure
      +- run cp transitive failure
         +- run mv transitive failure
            +- install pdex.elf transitive failure
               +- zig build-exe pdex.elf Debug thumb-freestanding-eabihf transitive failure
                  +- install pdex transitive failure
                     +- zig build-lib pdex Debug native 1 errors
src/main.zig:29:29: error: invalid builtin function: '@enumToInt'
```


* tried using `arch -x86_64 /bin/csh` but same error

* opened issue https://github.com/DanB91/Zig-Playdate-Template/issues/1

- turns out the built-ins got renamed. (@blahFromSplunge)


