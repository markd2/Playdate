const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");

var pd: *pdapi.PlaydateAPI = undefined;

pub const card = cardmod.Card{
    .name = "Collisions",
    .refreshHertz = 2,
    .init = init,
    .tick = tick
};


pub fn init(p: *pdapi.PlaydateAPI) void {
    pd = p;
}

pub fn draw() void {
    const white = @intFromEnum(pdapi.LCDSolidColor.ColorWhite);
    const black = @intFromEnum(pdapi.LCDSolidColor.ColorBlack);
    pd.graphics.clear(white);

    pd.graphics.drawRect(10, 50, 100, 75, black);
}


pub fn tick (deltaTime: u32) bool {
    _ = deltaTime;

    draw();
    return true;
}
