import Playdate

struct Game {
    var x: Int32 = 0
    var y: Int32 = 0
    

    init() {
        // Setup the device before any other operations.
        srand(System.getSecondsSinceEpoch(milliseconds: nil))
        Display.setRefreshRate(rate: 50)
    }
    
    mutating func updateGame() {
        sys.drawFPS(0, 0)

        let text: StaticString = "splunge"
        text.withUTF8Buffer { utf8 in
            _ = graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, x, y)
        }

        x += 10
        y += 10
        if x > 400 { x = rand() % 120 }
        if y > 250 { y = rand() % 50 }
    }
}

