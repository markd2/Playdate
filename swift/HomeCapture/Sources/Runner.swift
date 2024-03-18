import Playdate

class Runner: GameMode {
    override func reset() {
    }

    override func updateGame() -> State {
        pd.sys.drawFPS(0, 0)

        var pushed = PDButtons(rawValue: 0)
        pd.sys.getButtonState(nil, &pushed, nil)
        
        if pushed == .b {
            return .endGameWin
        }

        let text: StaticString = "Press (B) to Exit"
        text.withUTF8Buffer { utf8 in
            _ = pd.graphics.drawText(utf8.baseAddress, utf8.count, PDStringEncoding.kUTF8Encoding, 50, 50)
        }

        return .loco
    }
}
