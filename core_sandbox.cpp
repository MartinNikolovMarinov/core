#include "tests/t-index.h"

#include <iostream>

// f32 => .{
//     // Fast-Path
//     .min_exponent_fast_path = -10,
//     .max_exponent_fast_path = 10,
//     .max_exponent_fast_path_disguised = 17,
//     .max_mantissa_fast_path = 2 << std.math.floatMantissaBits(T),
//     // Slow + Eisel-Lemire
//     .mantissa_explicit_bits = std.math.floatFractionalBits(T),
//     .infinite_power = 0xff,
//     // Eisel-Lemire
//     .smallest_power_of_ten = -65,
//     .largest_power_of_ten = 38,
//     .minimum_exponent = -127,
//     .min_exponent_round_to_even = -17,
//     .max_exponent_round_to_even = 10,
// },
// f64 => .{
//     // Fast-Path
//     .min_exponent_fast_path = -22,
//     .max_exponent_fast_path = 22,
//     .max_exponent_fast_path_disguised = 37,
//     .max_mantissa_fast_path = 2 << std.math.floatMantissaBits(T),
//     // Slow + Eisel-Lemire
//     .mantissa_explicit_bits = std.math.floatMantissaBits(T),
//     .infinite_power = 0x7ff,
//     // Eisel-Lemire
//     .smallest_power_of_ten = -342,
//     .largest_power_of_ten = 308,
//     .minimum_exponent = -1023,
//     .min_exponent_round_to_even = -4,
//     .max_exponent_round_to_even = 23,
// },

// Fast-Path
constexpr i32 F32_MIN_EXPONENT_FAST_PATH = -10;
constexpr i32 F32_MAX_EXPONENT_FAST_PATH = 10;
constexpr i32 F32_MAX_EXPONENT_FAST_PATH_DISGUISED = 17;
// .max_mantissa_fast_path = 2 << std.math.floatMantissaBits(T),
// // Slow + Eisel-Lemire
// .mantissa_explicit_bits = std.math.floatFractionalBits(T),
// .infinite_power = 0xff,
// // Eisel-Lemire
// .smallest_power_of_ten = -65,
// .largest_power_of_ten = 38,
// .minimum_exponent = -127,
// .min_exponent_round_to_even = -17,
// .max_exponent_round_to_even = 10,

i32 main() {
    return 0;
}
