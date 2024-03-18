import Playdate

class GameMode {
    /// Major state to move to 
    enum State {
        case loco
        case introScreen
        case startGame
        case endGameWin
        case endGameLose
    }

    func updateGame() -> State {
        return .loco
    }

    func reset() {
    }
}

func clearScreen() {
    pd.graphics.clear(1)
}

struct Game {
    let splash = SplashScreen()
    let runner = Runner()

    var mode: GameMode

    init() {
        // Setup the device before any other operations.
        srand(System.getSecondsSinceEpoch(milliseconds: nil))
        Display.setRefreshRate(rate: 50)

        mode = splash
    }
    
    mutating func updateGame() {
        let state = mode.updateGame()
        switch state {
        case .loco:
            break
        case .introScreen:
            clearScreen()
            mode = splash
        case .startGame:
            clearScreen()
            mode = runner
        case .endGameWin:
            clearScreen()
            mode = splash
        case .endGameLose:
            clearScreen()
            mode = splash
        }
    }
}

