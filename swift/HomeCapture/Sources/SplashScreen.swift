import Playdate

class SplashScreen: GameMode {
    var x: Int32 = 0
    var y: Int32 = 0

    var text: StaticString  = "2D Home Capture"

    override func reset() {
        x = 0
        y = 0
        text = "2D Home Capture"
    }

    var frameSkip = 20

    override func updateGame() -> State {
        defer {
            frameSkip -= 1
            if frameSkip < 0 { frameSkip = 10 }
        }

        pd.sys.drawFPS(0, 0)
        
        if frameSkip == 0 {
            text.withUTF8Buffer { utf8 in
                _ = pd.graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, x, y)
            }
        
            x += 10
            y += 20
            if x > 400 { x = rand() % 120 }
            if y > 250 { y = rand() % 50 }
        }
        
        var pushed = PDButtons(rawValue: 0)
        pd.sys.getButtonState(nil, &pushed, nil)
        
        if pushed == .a {
            pd.graphics.clear(1)
            text = "A BUTTON"
            return .startGame
        } else if pushed == .b {
            pd.graphics.clear(1)
            text = "B BUTTON"
        } else {
            // pd.graphics.clear(1)
            text = ""
            blah(x: x, y: y)
        }

        return .loco
    }
}

func blah(x: Int32, y: Int32) {
    let bufferSize = 1024

    let phrase: [StaticString] = [
      "2D",
      " ",
      "Home",
      " ",
      "Capture"
    ]

    var scribble = UnsafeMutablePointer<CChar>.allocate(capacity: bufferSize)
    let base = scribble

    phrase.forEach { sstring in
        sstring.withUTF8Buffer { utf8 in
            scribble = stpcpy(scribble, utf8.baseAddress)
        }
    }

    let length = strlen(base)

    _ = pd.graphics.drawText(base, length, PDStringEncoding.kUTF8Encoding, x, y)
}

/*
// so close - CVarArg not defined, alas
func formatString(_ format: String, _ args: CVarArg...) -> [CChar] {
    var args = args
    let bufferSize = 1024
    var buffer = [CChar](repeating: 0, count: bufferSize)

    withVaList(args) { vaListPointer in
        // vsnprintf(&buffer, bufferSize, format, vaListPointer)
        pd.vaFormatString(&buffer, 
    }

    return buffer
}
*/
