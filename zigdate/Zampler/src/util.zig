const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");

var pd: *pdapi.PlaydateAPI = undefined;

pub fn setPd(pdx: *pdapi.PlaydateAPI) void {
    pd = pdx;
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

pub fn drawBitmap(bitmap: *pdapi.LCDBitmap, x: i16, y: i16) void {
    pd.graphics.drawBitmap(bitmap, x, y, .BitmapUnflipped);
}
