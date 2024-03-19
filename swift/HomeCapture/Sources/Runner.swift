// House images from https://www.etsy.com/shop/HeatherRobertsArt

import Playdate


struct Point {
    var x: Int32
    var y: Int32
}

let screenHeight: Int32 = 240
let screenWidth: Int32 = 400

// Stolen from swift-playdate-examples
extension Sprite {
    init(bitmapPath: StaticString) {
        let bitmap = Graphics.loadBitmap(path: bitmapPath)
        var width: Int32 = 0
        var height: Int32 = 0
        Graphics.getBitmapData(
          bitmap: bitmap,
          width: &width,
          height: &height,
          rowbytes: nil,
          mask: nil,
          data: nil)
        let bounds = PDRect(x: 0, y: 0, width: Float(width), height: Float(height))

        self.init()
        self.setImage(image: bitmap)
        self.bounds = bounds
        self.collideRect = bounds
    }
}


class Runner: GameMode {
    var netOrigin: Point = Point(x: 10, y: 10)

    let netWidth: Int32 = 20
    let netHeight: Int32 = 70

    var houseSprites: [Sprite]
    let houseSpriteNames: [StaticString] = [
      "house1.png",
      "house2.png",
      "house3.png",
      "house4.png",
      "house5.png",
      "house6.png",
      "house7.png",
      "house8.png",
      "house9.png",
      "house10.png",
      "house11.png",
      "house12.png",
      "house13.png",
      "house14.png",
      "house15.png",
      "house16.png",
      "house17.png",
      "house18.png"
    ]

    var currentLaunchIndex = 0

    override init() {
        houseSprites = houseSpriteNames.map { houseName in
            Sprite(bitmapPath: houseName)
        }
        super.init()
    }

    func kickOffSprite(_ sprite: Sprite) {
        sprite.moveTo(x: Float(screenWidth),
                      y: 50 + Float(rand() % screenHeight - 50))
        sprite.addSprite()
        sprite.setUpdateFunction { ptr in
            let sprite = Sprite(borrowing: ptr.unsafelyUnwrapped)
            let (x, y) = sprite.position
            let newX = x - 1
            sprite.moveTo(x: newX, y: y)
        }
    }

    override func reset() {
        netOrigin = Point(x: 10, y: 10)
    }

    private func drawNet(at: Point) {
        pd.graphics.setBackgroundColor(LCDSolidColor.colorBlack)

        pd.graphics.fillRect(at.x, at.y, netWidth, netHeight, 0)

        pd.graphics.setBackgroundColor(LCDSolidColor.colorWhite)
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
        Sprite.updateAndDrawSprites()

        drawNet(at: netOrigin)
        pd.sys.drawFPS(0, 0)

        // handle user input
        var pushed = PDButtons(rawValue: 0)
        pd.sys.getButtonState(nil, &pushed, nil)
        
        if pushed == .a {
            let sprite = houseSprites[currentLaunchIndex]
            currentLaunchIndex = (currentLaunchIndex + 1) % houseSprites.count
            kickOffSprite(sprite)
        }

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
