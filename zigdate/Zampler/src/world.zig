const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const RndGen = std.rand.DefaultPrng;
var rnd = RndGen.init(0);

pub const Robot = struct {
    row: u8,
    column: u8
};

pub const Pile = struct {
    row: u8,
    column: u8
};

pub const Player = struct {
    row: u8,
    column: u8,
    hasWeapon: bool
};

var robotCount: u8 = 0;
var robots: [30]Robot = undefined;

var pileCount: u8 = 0;
var piles: [30]Pile = undefined;

var player: Player = undefined;

const maxRows = 240 / 8;
const maxColumns = 400 / 8;

fn startGame() void {
    for (0..20) |i| {
        robots[i] = Robot{ .row = @mod(rnd.random().int(u8), maxRows),
                          .column = @mod(rnd.random().int(u8), maxColumns) };
    }
    player = Player{ .row = maxRows / 2, .column = maxColumns / 2,
                    .hasWeapon = true };
}

pub fn draw(pd: *pdapi.PlaydateAPI) void {
    _ = pd;
}
