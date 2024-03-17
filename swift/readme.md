# Swift and playdate

Woot! another non-lua non-C language we can poke around with.

* Byte-sized Swift: Building Tiny Games for the Playdate -- https://www.swift.org/blog/byte-sized-swift-tiny-games-playdate/
* Playdate examples repo -- https://github.com/apple/swift-playdate-examples
* Embedded Swift Vision -- https://github.com/apple/swift-evolution/blob/main/visions/embedded-swift.md

## Getting it working

Assuming you already have the Playdate SDK installed

### Getting the tools

Starting with "Downloading the tools": 
https://apple.github.io/swift-playdate-examples/documentation/playdate/downloadingthetools/

* go to the toolchains - https://www.swift.org/download/#snapshots
* Select xcrud "universal".  Around 1.3 gigs worth
* open and install (for everybody)

Finding installed toolchains:

```
% find /Library/Developer/Toolchains -depth 1 -print
/Library/Developer/Toolchains/swift-DEVELOPMENT-SNAPSHOT-2024-03-13-a.xctoolchain
/Library/Developer/Toolchains/swift-latest.xctoolchain
```

### Building

https://apple.github.io/swift-playdate-examples/documentation/playdate/buildingtheexamples

When using make, the latest installed Swift toolchain is automagically used.  Info at https://apple.github.io/swift-playdate-examples/documentation/playdate/buildingtheexamples/#Selecting-a-Toolchain if you need to select a 

and... it just worked.  omg.
