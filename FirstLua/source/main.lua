import "CoreLibs/graphics"
import "CoreLibs/sprites"
import "CoreLibs/timer"

-- macro to reduce callsite verbosity. "performance will be slightly enhanced, too"
local gfx <const> = playdate.graphics

local playerSprite = nil

function gameSetup()
    local playerImage = gfx.image.new("images/playerimage")
    assert(playerImage)

    playerSprite = gfx.sprite.new(playerImage)
    playerSprite:setCenter(0.5, 0.5)
    playerSprite:moveTo(200, 120)
    playerSprite:add() -- this is critical!

    local backgroundImage = gfx.image.new("images/backgroundimage")
    assert(backgroundImage)

    gfx.sprite.setBackgroundDrawingCallback(
        function(x, y, width, height)
            gfx.setClipRect(x, y, width, height) -- just draw what we need
            backgroundImage:draw(0, 0)
            gfx.clearClipRect()
        end
    )
end

gameSetup()

-- this is the heart of every game.  Call right before every frame is drawn
-- use to poll input, run game logic, move sprites, etc

function playdate.update()
    if playdate.buttonIsPressed(playdate.kButtonUp) then
        playerSprite:moveBy(0, -2)
    end
    if playdate.buttonIsPressed(playdate.kButtonRight) then
        playerSprite:moveBy(2, 0)
    end
    if playdate.buttonIsPressed(playdate.kButtonDown) then
        playerSprite:moveBy(0, 2)
    end
    if playdate.buttonIsPressed(playdate.kButtonLeft) then
        playerSprite:moveBy(-2, 0)
    end

    -- call to draw sprites and keep timers updated
    gfx.sprite.update()
    playdate.timer.updateTimers()
end


