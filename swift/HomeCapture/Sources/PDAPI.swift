import Playdate

enum pd {
    static var sys: playdate_sys { playdateAPI.system.unsafelyUnwrapped.pointee }
    static var file: playdate_file { playdateAPI.file.unsafelyUnwrapped.pointee }
    static var graphics: playdate_graphics { playdateAPI.graphics.unsafelyUnwrapped.pointee }
    static var sprite: playdate_sprite { playdateAPI.sprite.unsafelyUnwrapped.pointee }
    static var display: playdate_display { playdateAPI.display.unsafelyUnwrapped.pointee }
    static var sound: playdate_sound { playdateAPI.sound.unsafelyUnwrapped.pointee }
    static var lua: playdate_lua { playdateAPI.lua.unsafelyUnwrapped.pointee }
    static var json: playdate_json { playdateAPI.json.unsafelyUnwrapped.pointee }
    static var scoreboards: playdate_scoreboards { playdateAPI.scoreboards.unsafelyUnwrapped.pointee }
}

