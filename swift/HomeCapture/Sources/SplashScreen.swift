import Playdate

class SplashScreen: GameMode {
    var x: Int32 = 0
    var y: Int32 = 0

    var text: StaticString  = "splunge"

    override func reset() {
        x = 0
        y = 0
        text = "splunge"
    }

    override func updateGame() -> State {
        pd.sys.drawFPS(0, 0)
        
        text.withUTF8Buffer { utf8 in
            _ = pd.graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, x, y)
        }
        
        x += 10
        y += 10
        if x > 400 { x = rand() % 120 }
        if y > 250 { y = rand() % 50 }
        
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
            text = "Splunge"
        }

        return .loco
    }
}

