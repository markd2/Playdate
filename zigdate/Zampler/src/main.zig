const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const geo = @import("geometry.zig");
const world = @import("world.zig");

var g_playdate_image: *pdapi.LCDBitmap = undefined;

pub export fn eventHandler(pd: *pdapi.PlaydateAPI,
                           event: pdapi.PDSystemEvent,
                           arg: u32) callconv(.C) c_int {
    _ = arg;
    // const player = world.Player{ .row = 5, .column = 5, .hasWeapon = true };

    switch (event) {
        .EventInit => {

            g_playdate_image = pd.graphics.loadBitmap("playdate_image",
                                                      null).?;

            const font = pd.graphics.loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", null).?;
            pd.graphics.setFont(font);

            pd.system.setUpdateCallback(updateAndRender, pd);
        },
        .EventPause => {
            // setup menu
        },
        .EventResume => {
            // back from menu
        },
        else => {},
    }
    return 0;
}

fn mongoLog(pd: *pdapi.PlaydateAPI, comptime format: []const u8, args: anytype) void {
    const size = 0x100;
    const trunc_msg = "(msg truncated)";
    var buf: [size + trunc_msg.len]u8 = undefined;

    const msg = std.fmt.bufPrint(buf[0..size], format, args) catch |err| switch (err) {
        error.NoSpaceLeft => blk: {
            @memcpy(buf[size..], trunc_msg);
            break :blk &buf;
        },
    };

    // needing to go from the []u8 from bufPrint, to [*c]const u8
    // - [*c]T is just a C pointer to type T
    msg[msg.len - 1] = 0;
    var string: [*c]const u8 = @ptrCast(msg);

    pd.system.logToConsole(string);
}

fn updateAndRender(userdata: ?*anyopaque) callconv(.C) c_int {
    //TODO: replace with your own code!

    const pd: *pdapi.PlaydateAPI = @ptrCast(@alignCast(userdata.?));

    world.draw(pd);

    var current: pdapi.PDButtons = undefined;
    var pushed: pdapi.PDButtons = undefined;
    var released: pdapi.PDButtons = undefined;
    pd.system.getButtonState(&current, &pushed, &released);

    pd.graphics.clear(@intFromEnum(pdapi.LCDSolidColor.ColorWhite));

    if (current == 0) {
        _ = pd.graphics.drawText("BorkB", 5, .UTF8Encoding, 0, 0);
        pd.graphics.drawBitmap(g_playdate_image, 
                               pdapi.LCD_COLUMNS / 2 - 16, 
                               pdapi.LCD_ROWS / 2 - 16,
                               .BitmapUnflipped);
    } else {
        _ = pd.graphics.drawText("Greeble", 7, .UTF8Encoding, 0, 0);
        mongoLog(pd, "{b} {b} {b}\n", .{ current, pushed, released });

        const rp = geo.randomPoint();

        pd.graphics.drawBitmap(g_playdate_image, 
                               rp.x + pdapi.LCD_COLUMNS / 2 - 16, 
                               rp.y + pdapi.LCD_ROWS / 2 - 16,
                               .BitmapUnflipped);
    }


    // returning 1 signals to the OS to draw the frame.
    // we always want this frame drawn.
    return 1;
}
