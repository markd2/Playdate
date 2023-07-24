const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const geo = @import("geometry.zig");
const world = @import("world.zig");
const card = @import("card.zig");

const wrapper = @import("playdate_api_wrapper.zig");

var g_playdate_image: *pdapi.LCDBitmap = undefined;
var pd: *pdapi.PlaydateAPI = undefined;

pub export fn eventHandler(pd_in: *pdapi.PlaydateAPI, event: pdapi.PDSystemEvent, arg: u32) callconv(.C) c_int {
    _ = arg;
    // const player = world.Player{ .row = 5, .column = 5, .hasWeapon = true };

    switch (event) {
        .EventInit => {
            pd = pd_in;
            g_playdate_image = pd.graphics.loadBitmap("playdate_image", null).?;

            const font = pd.graphics.loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", null).?;
            pd.graphics.setFont(font);

            world.init(pd);
            world.startGame();

            pd.system.setUpdateCallback(updateAndRender, pd);
        },
        .EventPause => {
            setupMenu();
        },
        .EventResume => {
            // back from menu
        },
        else => {},
    }
    return 0;
}

fn menuItemCallback(userdata: ?*anyopaque) callconv(.C) void {
    _ = userdata;
}

fn setupMenu() void {
    // set up a an "options" menu of all the available cards
    // In The Future: maybe the card can supply a menu item or a checkmark item
    // pd.system.removeAllMenuItems();

    wrapper.set_playdate_api(pd);
    const string: [:0]const u8 = "hello";
    const jello: [:0]const u8 = "jello";
    const snarnge: [:0]const u8 = "snarnge";
    var strings = [_][*c]const u8{ string.ptr, jello.ptr, snarnge.ptr };

    const menuItem = wrapper.add_options_menu_item(
        "Title",
        menuItemCallback,
        &strings,
        null);
    _ = menuItem;
}

pub fn mongoLog(comptime format: []const u8, args: anytype) void {
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
    _ = userdata; // this is the playdate api, but we have a global we can access.

    world.draw(pd);

    var current: pdapi.PDButtons = 0;
    var pushed: pdapi.PDButtons = 0;
    var released: pdapi.PDButtons = 0;
    pd.system.getButtonState(&current, &pushed, &released);

    if (pushed != 0) {
        mongoLog("pooshed {b}", .{pushed});
        world.startGame();
    }

    if (world.tick(current, pushed, released)) {
        world.draw(pd);
        // returning 1 signals to the OS to draw the frame.
        return 1;
    } else {
        return 0;
    }
}
