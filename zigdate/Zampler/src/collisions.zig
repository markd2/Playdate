const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");

pub const card = cardmod.Card{
    .name = "Collisions",
    .draw = draw
};

pub fn draw(pd: *pdapi.PlaydateAPI) void {
    const white = @intFromEnum(pdapi.LCDSolidColor.ColorWhite);
    const black = @intFromEnum(pdapi.LCDSolidColor.ColorBlack);
    pd.graphics.clear(white);

    pd.graphics.drawRect(10, 50, 100, 75, black);
}

