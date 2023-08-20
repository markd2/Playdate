// Simple animation

const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const cardmod = @import("card.zig");
const util = @import("util.zig");

var pd: *pdapi.PlaydateAPI = undefined;

// todo - unify these constants
pub const kButtonRight = pdapi.BUTTON_RIGHT;
pub const kButtonLeft = pdapi.BUTTON_LEFT;

var g_robot_image: *pdapi.LCDBitmap = undefined;
var g_player_image: *pdapi.LCDBitmap = undefined;


pub const card = cardmod.Card{
    .name = "Animator",
    .refreshHertz = 10,
    .init = init,
    .tick = tick
};

const Point = struct {
    x: i16,
    y: i16
};

const Anim = struct {
    type: u8,

    start: Point,
    end: Point,

    fn location(self: Anim, percentage: u8) Point {
        const deltax = self.end.x - self.start.x;
        const deltay = self.end.y - self.start.y;

        const newx = self.start.x + @divTrunc((deltax * percentage), 100);
        const newy = self.start.y + @divTrunc((deltay * percentage), 100);

        return Point{ .x = newx, .y = newy };
    }
};

const anim1 = Anim{ .type = 0,
                   .start = Point{ .x = 10, .y = 10 },
                   .end = Point{ .x = 200, .y = 10 } };

const anim2 = Anim{ .type = 1,
                   .start = Point{ .x = 10, .y = 20 },
                   .end = Point{ .x = 150, .y = 150 } };
const anims: [2]Anim = .{ anim1, anim2 };

pub fn init(p: *pdapi.PlaydateAPI) void {
    pd = p;

    g_robot_image = pd.graphics.loadBitmap("robot", null).?;
    g_player_image = pd.graphics.loadBitmap("player", null).?;
}

fn drawEnd(point: Point) void {
    const black = @intFromEnum(pdapi.LCDSolidColor.ColorBlack);
    pd.graphics.drawEllipse(point.x, point.y, 10, 10,
                            1, 0.0, 360.0, black);
}

pub fn draw() void {
    const white = @intFromEnum(pdapi.LCDSolidColor.ColorWhite);
    pd.graphics.clear(white);

    const text = "(A) to animate.   (< >) to Step";

    const x = 5;
    const y = 220;
    _ = pd.graphics.drawText(text.ptr, text.len, .ASCIIEncoding, x, y);

    for (anims) | anim | {
        const p = anim.location(percent);
        switch (anim.type) {
            0 => util.drawBitmap(g_player_image, p.x, p.y),
            1 => util.drawBitmap(g_robot_image, p.x, p.y),
            else => {}
        }
        drawEnd(anim.end);
    }
}

var percent: u8 = 0;

pub fn tick (deltaTime: u32) bool {
    _ = deltaTime;

    var current: pdapi.PDButtons = undefined;
    pd.system.getButtonState(&current, null, null);

    if (current & kButtonLeft != 0) {
        percent -%= 10;
        if (percent > 100) percent = 100;
    }
    if (current & kButtonRight != 0) {
        percent += 10;
        if (percent > 100) percent = 0;
    }

    draw();
    return true;
}

