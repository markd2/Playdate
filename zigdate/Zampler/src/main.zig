const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");
const geo = @import("geometry.zig");
const card = @import("card.zig");
const wrapper = @import("playdate_api_wrapper.zig");

// Cards
const robots = @import("robots.zig");
const collisions = @import("collisions.zig");
const robotsCard = robots.card;
const collisionsCard = collisions.card;
// !!! not a fan of the hardcoded 2
const allCards: [2]card.Card = .{ robotsCard, collisionsCard };
var currentCard: card.Card = robotsCard;

var g_playdate_image: *pdapi.LCDBitmap = undefined;
var pd: *pdapi.PlaydateAPI = undefined;

var optionsMenu: *pdapi.PDMenuItem = undefined;
var lastTime: u32 = undefined;

pub export fn eventHandler(pd_in: *pdapi.PlaydateAPI, event: pdapi.PDSystemEvent, arg: u32) callconv(.C) c_int {
    _ = arg;
    // const player = robots.Player{ .row = 5, .column = 5, .hasWeapon = true };

    switch (event) {
        .EventInit => {
            pd = pd_in;
            g_playdate_image = pd.graphics.loadBitmap("playdate_image", null).?;

            const font = pd.graphics.loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", null).?;
            pd.graphics.setFont(font);

            lastTime = @as(u32, pd.system.getCurrentTimeMilliseconds());

            robotsCard.init(pd);
            collisionsCard.init(pd);

            setupMenu();

            pd.system.setUpdateCallback(updateAndRender, pd);
        },
        .EventPause => {
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

    var chosenOption: c_uint = @as(c_uint, @intCast(pd.system.getMenuItemValue(optionsMenu)));
//    const chosenOption = @truncate(u32, pd.system.getMenuItemValue(optionsMenu));
    currentCard = allCards[chosenOption];
}

fn setupMenu() void {
    // set up a an "options" menu of all the available cards
    // In The Future: maybe the card can supply a menu item or a checkmark item
    // pd.system.removeAllMenuItems();

    wrapper.set_playdate_api(pd);
    var strings = [_][*c]const u8{
        robotsCard.name.ptr,
        collisionsCard.name.ptr };

    optionsMenu = wrapper.add_options_menu_item(
        "Title",
        menuItemCallback,
        &strings,
        null);
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

    var now = @as(u32, pd.system.getCurrentTimeMilliseconds());
    var delta = now - lastTime;
    lastTime = now;

    return if (currentCard.tick(delta)) 1 else 0;
}
