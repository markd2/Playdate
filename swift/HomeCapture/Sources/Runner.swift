import Playdate

struct Point {
    var x: Int32
    var y: Int32
}

let screenHeight: Int32 = 240
let screenWidth: Int32 = 400


class Runner: GameMode {
    var netOrigin: Point = Point(x: 10, y: 10)

    let netWidth: Int32 = 20
    let netHeight: Int32 = 70

    override func reset() {
        netOrigin = Point(x: 10, y: 10)
    }

    private func drawNet(at: Point) {
        pd.graphics.setBackgroundColor(LCDSolidColor.colorBlack)

        pd.graphics.fillRect(at.x, at.y, netWidth, netHeight, 0)
    }

    private func moveNet(by delta: Int32) {
        var candidate = Point(x: netOrigin.x, y: netOrigin.y + delta)

        if candidate.y + netHeight > screenHeight {
            // clamp
            candidate.y = screenHeight - netHeight
        }

        if candidate.y < 0 {
            // clamp
            candidate.y = 0
        }

        // maybe x-direction too?

        netOrigin = candidate
    }

    override func updateGame() -> State {

        // draw stuff
        clearScreen()
        drawNet(at: netOrigin)
        pd.sys.drawFPS(0, 0)

        // handle user input
        var pushed = PDButtons(rawValue: 0)
        pd.sys.getButtonState(nil, &pushed, nil)
        
        if pushed == .b {
            // game over man, GAME OVER

            // a winner is you!
            return .endGameWin
        }

        let text: StaticString = "Press (B) to Exit"
        text.withUTF8Buffer { utf8 in
            _ = pd.graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, 50, 50)
        }

        let delta = pd.sys.getCrankChange()
        moveNet(by: Int32(delta))

        // stick with us, we got another frame to process
        return .loco
    }
}
