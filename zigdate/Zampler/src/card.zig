const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");

pub const Card = struct {
    name: [:0]const u8,
    draw: *const fn (pd: *pdapi.PlaydateAPI) void
};
