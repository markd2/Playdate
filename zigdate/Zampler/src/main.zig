const std = @import("std");
const pdapi = @import("playdate_api_definitions.zig");

var g_playdate_image: *pdapi.LCDBitmap = undefined;

pub export fn eventHandler(pd: *pdapi.PlaydateAPI,
                           event: pdapi.PDSystemEvent,
                           arg: u32) callconv(.C) c_int {
    _ = arg;

    switch (event) {
        .EventInit => {
            g_playdate_image = pd.graphics.loadBitmap("pd_image",
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

fn updateAndRender(userdata: ?*anyopaque) callconv(.C) c_int {
    //TODO: replace with your own code!

    const pd: *pdapi.PlaydateAPI = @ptrCast(@alignCast(userdata.?));
    const to_draw = "Hello from Zig!";

    pd.graphics.clear(@intFromEnum(pdapi.LCDSolidColor.ColorWhite));

    _ = pd.graphics.drawText(to_draw, to_draw.len, .UTF8Encoding, 0, 0);

    pd.graphics.drawBitmap(g_playdate_image, 
                           pdapi.LCD_COLUMNS / 2 - 16, 
                           pdapi.LCD_ROWS / 2 - 16,
                           .BitmapUnflipped);

    //returning 1 signals to the OS to draw the frame.
    //we always want this frame drawn
    return 1;
}
