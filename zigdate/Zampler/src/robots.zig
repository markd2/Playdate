const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");
const util = @import("util.zig");

const RndGen = std.rand.DefaultPrng;

var pd: *pdapi.PlaydateAPI = undefined;

pub const kButtonLeft = pdapi.BUTTON_LEFT;
pub const kButtonRight = pdapi.BUTTON_RIGHT;
pub const kButtonUp = pdapi.BUTTON_UP;
pub const kButtonDown = pdapi.BUTTON_DOWN;

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

const initialRobotCount = 20;

var robotCount: u8 = 0;
var robots: [30]Robot = undefined;

var pileCount: u8 = 0;
var piles: [30]Pile = undefined;

var player: Player = undefined;

const cellSize = 8;
const maxRows = 240 / cellSize;
const maxColumns = 400 / cellSize;

// var playfield: [maxRows * maxColumns]u8 = {0} ** (maxRows * maxColumns);
var playfield= [_]u8{0} ** (maxRows * maxColumns);

fn indexOf(row: u16, column: u16) u16 {
    return row * maxColumns + column;
}

const kEmpty = 0;
const kRobot = 1;
const kPlayer = 2;

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

        if (playfield[indexOf(row, column)] != kEmpty) {
            continue;
        }

        // give the player some breathing room
        const playerSpace = 3;
        if (row >= playerRow - playerSpace
                and row <= playerRow + playerSpace
                and column >= playerColumn - playerSpace
                and column <= playerColumn + playerSpace) {
            continue;
        }
        
        robots[robotCount] = Robot{ .row = row, .column = column };

        playfield[indexOf(row, column)] = kRobot;
        
        robotCount += 1;
    }

    player = Player{ .row = playerRow, .column = playerColumn,
                    .hasWeapon = true };
    playfield[indexOf(playerRow, playerColumn)] = kPlayer;
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


var lastCurrent: pdapi.PDButtons = -1;

pub fn tick (deltaTime: u32) bool {
    _ = deltaTime;

    var current: pdapi.PDButtons = undefined;
    pd.system.getButtonState(&current, null, null);

    var redraw = false;

    if (lastCurrent == -1) {
        redraw = true;
    }

    if (current != lastCurrent) {
        
        if (current & kButtonLeft != 0) {
            util.mongoLog("left huh what's going on?", .{});
            player.column -= 1;
        }
        if (current & kButtonRight != 0) {
            util.mongoLog("right", .{});
            player.column += 1;
        }
        if (current & kButtonUp != 0) {
            util.mongoLog("up ", .{});
            player.row -= 1;
        }
        if (current & kButtonDown != 0) {
            util.mongoLog("down ", .{});
            player.row += 1;
        }

        lastCurrent = current;
        redraw = true;
        util.mongoLog("--------", .{});
    }

    if (redraw) {
        draw();
    }
    return redraw;
}
