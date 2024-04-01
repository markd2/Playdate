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

## printing

logToConsole isn't available because it's a C variadic?

Not sure how to caveman debug this thing

## Protocols

Trying to do a protocol thing

```
protocol GameMode {
    mutating func updateGame()
}
```

and then

```
struct Game {
    var mode: GameMode = SplashScreen()
```

getting

```
cannot use a value of protocol type 'any GameMode' in embedded Swift
```

"protocol" not mentioned at all in the example repo, so...

Amusingly enough, couldn't figure out how to use protocols, but inheritance
works just fine (with the overhead of a dynamic allocation, but for the
game state machine, should only happen once)


### I can hazs string?

Sources/SplashScreen.swift:56:45: error: cannot find type 'CVarArg' in scope

starting with

```
    func formatString(_ format: String, _ args: CVarArg...) -> [CChar] {
        var args = args
        let bufferSize = 1024
        var buffer = [CChar](repeating: 0, count: bufferSize)
        
        withVaList(args) { vaListPointer in
            vsnprintf(&buffer, bufferSize, format, vaListPointer)
            // pd.vaFormatString(&buffer, 
        }
        
        return buffer
    }
```

```
56 │ func formatString(_ format: String, _ args: CVarArg...) -> [CChar] {
   │                                             ╰─ error: cannot find type 'CVarArg' in scope
```

This works.  Not pretty, and definitely not safe.  But does let us catenate
strings together.

```
    let bufferSize = 1024

    let blah: StaticString = "hello"
    let space: StaticString = " "
    let oop: StaticString = "freaks"

    var scribble = UnsafeMutablePointer<CChar>.allocate(capacity: 1024)
    let base = scribble

    blah.withUTF8Buffer { utf8 in
        scribble = stpcpy(scribble, utf8.baseAddress)
    }
    space.withUTF8Buffer { utf8 in
        scribble = stpcpy(scribble, utf8.baseAddress)
    }
    oop.withUTF8Buffer { utf8 in
        scribble = stpcpy(scribble, utf8.baseAddress)
    }

    let length = strlen(base)

    _ = pd.graphics.drawText(base, length, PDStringEncoding.kUTF8Encoding, 100, 50)
```

Closure capture:

When  I try to capture in a closure:

```
1.	Apple Swift version 6.0-dev (LLVM d1625da873daa4c, Swift bae6450bf96dceb)
2.	Compiling with the current language version
3.	While evaluating request ExecuteSILPipelineRequest(Run pipelines { Mandatory Diagnostic Passes + Enabling Optimization Passes } on SIL for game_device)
4.	While running pass #1233 SILFunctionTransform "TransferNonSendable" on SILFunction "@$s11game_device6RunnerC13kickOffSpriteyy09playdate_B00F0OF".
 for 'kickOffSprite(_:)' (at Sources/Runner.swift:112:5)
Stack dump without symbol names (ensure you have llvm-symbolizer in your PATH or set the environment var `LLVM_SYMBOLIZER_PATH` to point to it):
```

