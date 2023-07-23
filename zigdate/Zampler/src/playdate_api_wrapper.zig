// From DanB91 upward source

const std = @import("std");
const pddefs = @import("playdate_api_definitions.zig");

var pd: *pddefs.PlaydateAPI = undefined;

pub inline fn set_playdate_api(playdate: *pddefs.PlaydateAPI) void {
    pd = playdate;
}

pub inline fn add_options_menu_item(
    title: [:0]const u8,
    callback: ?pddefs.PDMenuItemCallbackFunction,
    option_titles: [][*c]const u8,
    userdata: ?*anyopaque,
) *pddefs.PDMenuItem {
    return pd.system.addOptionsMenuItem(
        title.ptr,
        option_titles.ptr,
        @as(c_int, @intCast(option_titles.len)),
        callback,
        userdata,
    ).?;
}
