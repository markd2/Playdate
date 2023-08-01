const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");

var pd: *pdapi.PlaydateAPI = undefined;

pub const card = cardmod.Card{
    .name = "Collisions",
    .init = init,
    .draw = draw
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

