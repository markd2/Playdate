import Playdate

var sys: playdate_sys { playdateAPI.system.unsafelyUnwrapped.pointee }

struct Game {

    init() {
        // Setup the device before any other operations.
        srand(System.getSecondsSinceEpoch(milliseconds: nil))
        Display.setRefreshRate(rate: 50)
    }
    
    mutating func updateGame() {
        sys.drawFPS(0, 0)
    }
}

