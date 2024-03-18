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


## Fun stuff.

### API Tables

If try to use the playdate API pointer, get the concurrency smackdown:

```
10 │     mutating func updateGame() {
11 │         playdate.pointee.system.pointee.drawFPS(0, 0)
   │         ╰─ error: reference to var 'playdate' is not concurrency-safe because it involves shared mutable state
```

but luckily can do something like this:

```
var sys: playdate_sys { playdateAPI.system.unsafelyUnwrapped.pointee }
```

and then do

```
    mutating func updateGame() {
        sys.drawFPS(0, 0)
```

### Drawing a string

No String available:

```
17 │         let text = "splunge"
   │                    ╰─ error: 'String' is unavailable: unavailable in embedded Swift
Swift.String:2:15: note: 'String' has been explicitly marked unavailable here
1 │ @available(*, unavailable, message: "unavailable in embedded Swift")
2 │ public struct String : Hashable {
  │               ╰─ note: 'String' has been explicitly marked unavailable here
```

StaticString exists, which is easy (Well, Swift-Easy) to get bytes from:

```
    let text: StaticString = "splunge"
        
    text.withUTF8Buffer { utf8 in
        _ = graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, x, y)
    }
```
