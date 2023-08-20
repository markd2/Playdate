// Simple animation

const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");
const util = @import("util.zig");

var pd: *pdapi.PlaydateAPI = undefined;

var g_robot_image: *pdapi.LCDBitmap = undefined;
var g_player_image: *pdapi.LCDBitmap = undefined;


pub const card = cardmod.Card{
    .name = "Animator",
    .refreshHertz = 10,
    .init = init,
    .tick = tick
};


pub fn init(p: *pdapi.PlaydateAPI) void {
    pd = p;

    g_robot_image = pd.graphics.loadBitmap("robot", null).?;
    g_player_image = pd.graphics.loadBitmap("player", null).?;
}

pub fn draw() void {
    const white = @intFromEnum(pdapi.LCDSolidColor.ColorWhite);
    pd.graphics.clear(white);

    util.drawBitmap(g_player_image, 10, 10);
    util.drawBitmap(g_robot_image, 60, 80);
}

pub fn tick (deltaTime: u32) bool {
    _ = deltaTime;

    draw();
    return true;
}

