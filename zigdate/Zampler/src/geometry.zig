const std = @import("std");
const RndGen = std.rand.DefaultPrng;

var rnd = RndGen.init(0);

pub const Point = struct {
    x: i16,
    y: i16
};

pub fn randomPoint() Point {
    const x = @mod(rnd.random().int(i16), 50) - 25;
    const y = @mod(rnd.random().int(i16), 50) - 25;
    return Point{.x = x, .y = y};
}

