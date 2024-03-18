import Playdate

var sys: playdate_sys { playdateAPI.system.unsafelyUnwrapped.pointee }
var file: playdate_file { playdateAPI.file.unsafelyUnwrapped.pointee }
var graphics: playdate_graphics { playdateAPI.graphics.unsafelyUnwrapped.pointee }
var sprite: playdate_sprite { playdateAPI.sprite.unsafelyUnwrapped.pointee }
var display: playdate_display { playdateAPI.display.unsafelyUnwrapped.pointee }
var sound: playdate_sound { playdateAPI.sound.unsafelyUnwrapped.pointee }
var lua: playdate_lua { playdateAPI.lua.unsafelyUnwrapped.pointee }
var json: playdate_json { playdateAPI.json.unsafelyUnwrapped.pointee }
var scoreboards: playdate_scoreboards { playdateAPI.scoreboards.unsafelyUnwrapped.pointee }

