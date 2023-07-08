const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const RndGen = std.rand.DefaultPrng;

var rnd = RndGen.init(0);

var g_robot_image: *pdapi.LCDBitmap = undefined;

pub const Robot = struct {
    row: u16,
    column: u16
};

pub const Pile = struct {
    row: u16,
    column: u16
};

pub const Player = struct {
    row: u16,
    column: u16,
    hasWeapon: bool
};

const initialRobotCount = 30;

var robotCount: u8 = 0;
var robots: [30]Robot = undefined;

var pileCount: u8 = 0;
var piles: [30]Pile = undefined;

var player: Player = undefined;

const cellSize = 8;
const maxRows = 240 / cellSize;
const maxColumns = 400 / cellSize;

pub fn init(pd: *pdapi.PlaydateAPI) void {
    g_robot_image = pd.graphics.loadBitmap("robot", null).?;
}

pub fn startGame() void {
    robotCount = 0;

    for (0..initialRobotCount) |i| {
        robots[i] = Robot{ .row = @mod(rnd.random().int(u16), maxRows),
                          .column = @mod(rnd.random().int(u16), maxColumns) };
        robotCount += 1;
    }

    player = Player{ .row = maxRows / 2, .column = maxColumns / 2,
                    .hasWeapon = true };
}

// returns true if things need redrawing
pub fn tick(current: pdapi.PDButtons,
            pushed: pdapi.PDButtons,
            released: pdapi.PDButtons) bool {
    _ = current;
    _ = pushed;
    _ = released;
    return true;
}


pub fn draw(pd: *pdapi.PlaydateAPI) void {
    pd.graphics.clear(@intFromEnum(pdapi.LCDSolidColor.ColorWhite));
    
    for (0..robotCount) |i| {
        const row = robots[i].row;
        const col = robots[i].column;
        const x = col * cellSize;
        const y = row * cellSize;
        
        pd.graphics.drawBitmap(g_robot_image, x, y, .BitmapUnflipped);
    }
}
