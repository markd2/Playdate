const card = @import("card.zig");
const geo = @import("geometry.zig");
const pdapi = @import("playdate_api_definitions.zig");
const std = @import("std");
const util = @import("util.zig");
const wrapper = @import("playdate_api_wrapper.zig");

// Cards
const animator = @import("animator.zig");
const collisions = @import("collisions.zig");
const robots = @import("robots.zig");

const animatorCard = animator.card;
const collisionsCard = collisions.card;
const robotsCard = robots.card;

// !!! not a fan of the hardcoded ~2~ 3
const allCards: [3]card.Card = .{ animatorCard, robotsCard, collisionsCard };
var currentCard: card.Card = undefined;

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
            util.setPd(pd);
            g_playdate_image = pd.graphics.loadBitmap("playdate_image", null).?;

            const font = pd.graphics.loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", null).?;
            pd.graphics.setFont(font);

            lastTime = @as(u32, pd.system.getCurrentTimeMilliseconds());

            animatorCard.init(pd);
            robotsCard.init(pd);
            collisionsCard.init(pd);

            moveToCard(allCards[0]);

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

fn moveToCard(newCard: card.Card) void {
    currentCard = newCard;
    const rate = @as(f32, @floatFromInt(currentCard.refreshHertz));
    pd.display.setRefreshRate(rate);
}

fn menuItemCallback(userdata: ?*anyopaque) callconv(.C) void {
    _ = userdata;

    var chosenOption: c_uint = @as(c_uint, @intCast(pd.system.getMenuItemValue(optionsMenu)));
    moveToCard(allCards[chosenOption]);
}

fn setupMenu() void {
    // set up a an "options" menu of all the available cards
    // In The Future: maybe the card can supply a menu item or a checkmark item
    // pd.system.removeAllMenuItems();

    wrapper.set_playdate_api(pd);
    var strings = [_][*c]const u8{
        animatorCard.name.ptr,
        robotsCard.name.ptr,
        collisionsCard.name.ptr };

    optionsMenu = wrapper.add_options_menu_item(
        "Title",
        menuItemCallback,
        &strings,
        null);
}

fn updateAndRender(userdata: ?*anyopaque) callconv(.C) c_int {
    _ = userdata; // this is the playdate api, but we have a global we can access.

    var now = @as(u32, pd.system.getCurrentTimeMilliseconds());

    var delta = now - lastTime;
    lastTime = now;

    return if (currentCard.tick(delta)) 1 else 0;
}
