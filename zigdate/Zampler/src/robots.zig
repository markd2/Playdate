const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");

const RndGen = std.rand.DefaultPrng;

var pd: *pdapi.PlaydateAPI = undefined;

pub const card = cardmod.Card{
    .name = "Robots",
    .init = init,
    .tick = tick
};

var rnd = RndGen.init(0);

var g_robot_image: *pdapi.LCDBitmap = undefined;
var g_player_image: *pdapi.LCDBitmap = undefined;

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

pub fn init(p: *pdapi.PlaydateAPI) void {
    pd = p;

    g_robot_image = pd.graphics.loadBitmap("robot", null).?;
    g_player_image = pd.graphics.loadBitmap("player", null).?;
    robotCount = 0;

    var playerRow: u8 = maxRows / 2;
    var playerColumn: u8 = maxColumns / 2;

    while (robotCount < initialRobotCount) {

        const row = @mod(rnd.random().int(u16), maxRows);
        const column = @mod(rnd.random().int(u16), maxColumns);

        // give the player some breathing room
        const playerSpace = 3;
        if (row >= playerRow - playerSpace
                and row <= playerRow + playerSpace
                and column >= playerColumn - playerSpace
                and column <= playerColumn + playerSpace) {
            continue;
        }
        
        robots[robotCount] = Robot{ .row = row, .column = column };
        
        robotCount += 1;
    }

    player = Player{ .row = playerRow, .column = playerColumn,
                    .hasWeapon = true };
}


pub fn draw() void {
    pd.graphics.clear(@intFromEnum(pdapi.LCDSolidColor.ColorWhite));
    
    for (0..robotCount) |i| {
        const row = robots[i].row;
        const col = robots[i].column;
        const x = col * cellSize;
        const y = row * cellSize;
        
        pd.graphics.drawBitmap(g_robot_image, x, y, .BitmapUnflipped);
    }

    const row = player.row;
    const col = player.column;
    const x = col * cellSize;
    const y = row * cellSize;
        
    pd.graphics.drawBitmap(g_player_image, x, y, .BitmapUnflipped);
}


pub fn tick (deltaTime: u32) bool {
    _ = deltaTime;
    draw();
    return true;
}
