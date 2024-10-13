#include "../t-index.h"

struct __Wrapping_Math_TestCase {
    enum Type: u8 {
        I8, I16, I32, I64,
        U8, U16, U32, U64,
    };

    struct Integer { // NOTE: Can't be a union becaue I want to use this in constexpr tests.
        i8 as_i8 = 123;
        i16 as_i16 = 123;
        i32 as_i32 = 123;
        i64 as_i64 = 123;
        u8 as_u8 = 123;
        u16 as_u16 = 123;
        u32 as_u32 = 123;
        u64 as_u64 = 123;

        constexpr Integer(i8 v)  : as_i8(v) {}
        constexpr Integer(i16 v) : as_i16(v) {}
        constexpr Integer(i32 v) : as_i32(v) {}
        constexpr Integer(i64 v) : as_i64(v) {}
        constexpr Integer(u8 v)  : as_u8(v) {}
        constexpr Integer(u16 v) : as_u16(v) {}
        constexpr Integer(u32 v) : as_u32(v) {}
        constexpr Integer(u64 v) : as_u64(v) {}
    };

    Type type;
    Integer a;
    Integer b;
    Integer want;
    bool noWrapAround;
};

constexpr __Wrapping_Math_TestCase __wrapping_add_testCases[] = {
    // I8 no wrap around
    { __Wrapping_Math_TestCase::Type::I8, i8(0), i8(0), i8(0), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(5), i8(10), i8(15), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(10), i8(-5), i8(5), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-5), i8(-5), i8(-10), true },
    { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), core::limitMin<i8>(), i8(-1), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>() - 1), i8(1), i8(core::limitMax<i8>()), true },

    // I8 wrap around
    { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), i8(1), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), core::limitMax<i8>(), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, core::limitMin<i8>(), i8(-1), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, core::limitMin<i8>(), core::limitMin<i8>(), i8(0), false },

    // U8 no wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(0), u8(0), u8(0), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(5), u8(10), u8(15), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>() - 1), u8(1), u8(core::limitMax<u8>()), true },

    // U8 wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(1), u8(0), false },
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(core::limitMax<u8>()), u8(0), false },

    // I16 no wrap around
    { __Wrapping_Math_TestCase::Type::I16, i16(0), i16(0), i16(0), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(5), i16(10), i16(15), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(10), i16(-5), i16(5), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(-10), i16(-5), i16(-15), true },
    { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), core::limitMin<i16>(), i16(-1), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>() - 1), i16(1), i16(core::limitMax<i16>()), true },

    // I16 wrap around
    { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), i16(1), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), core::limitMax<i16>(), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, core::limitMin<i16>(), i16(-1), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, core::limitMin<i16>(), core::limitMin<i16>(), i16(0), false },

     // U16 no wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(0), u16(0), u16(0), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(5), u16(10), u16(15), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>() - 1), u16(1), u16(core::limitMax<u16>()), true },

    // U16 wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(1), u16(0), false },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(core::limitMax<u16>()), u16(0), false },

    // I32 no wrap around
    { __Wrapping_Math_TestCase::Type::I32, i32(0), i32(0), i32(0), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(5), i32(10), i32(15), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(10), i32(-5), i32(5), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(-10), i32(-5), i32(-15), true },
    { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), core::limitMin<i32>(), i32(-1), true },
    { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>() - 1, i32(1), core::limitMax<i32>(), true },

    // I32 wrap around
    { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), i32(1), i32(0), false },
    { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), core::limitMax<i32>(), i32(0), false },
    { __Wrapping_Math_TestCase::Type::I32, core::limitMin<i32>(), i32(-1), i32(0), false },
    { __Wrapping_Math_TestCase::Type::I32, core::limitMin<i32>(), core::limitMin<i32>(), i32(0), false },

    // U32 no wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(0), u32(0), u32(0), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(5), u32(10), u32(15), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>() - 1), u32(1), u32(core::limitMax<u32>()), true },

    // U32 wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(1), u32(0), false },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(core::limitMax<u32>()), u32(0), false },

    // I64 no wrap around
    { __Wrapping_Math_TestCase::Type::I64, i64(0), i64(0), i64(0), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(5), i64(10), i64(15), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(10), i64(-5), i64(5), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-10), i64(-5), i64(-15), true },
    { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), core::limitMin<i64>(), i64(-1), true },
    { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>() - 1, i64(1), core::limitMax<i64>(), true },

    // I64 wrap around
    { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), i64(1), i64(0), false },
    { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), core::limitMax<i64>(), i64(0), false },
    { __Wrapping_Math_TestCase::Type::I64, core::limitMin<i64>(), i64(-1), i64(0), false },
    { __Wrapping_Math_TestCase::Type::I64, core::limitMin<i64>(), core::limitMin<i64>(), i64(0), false },

    // U64 no wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(0), u64(0), u64(0), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(5), u64(10), u64(15), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>() - 1), u64(1), u64(core::limitMax<u64>()), true },

    // U64 wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(1), u64(0), false },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(core::limitMax<u64>()), u64(0), false },
};

constexpr __Wrapping_Math_TestCase __wrapping_sub_testCases[] = {
    // I8 no wrap around
    { __Wrapping_Math_TestCase::Type::I8, i8(0), i8(0), i8(0), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(11), i8(5), i8(6), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(17), i8(-5), i8(22), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-5), i8(-12), i8(7), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), core::limitMax<i8>(), i8(0), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>() + 1), i8(1), i8(core::limitMin<i8>()), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>()), i8(core::limitMin<i8>()), i8(0), true },

    // I8 wrap around
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), i8(-1), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), i8(-core::limitMax<i8>()), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>()), i8(1), i8(0), false },

    // U8 no wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(0), u8(0), u8(0), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(13), u8(10), u8(3), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMin<u8>() + 1), u8(1), u8(core::limitMin<u8>()), true },

    // U8 wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMin<u8>()), u8(1), u8(0), false },

    // I16 no wrap around
    { __Wrapping_Math_TestCase::Type::I16, i16(0), i16(0), i16(0), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(11), i16(5), i16(6), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(17), i16(-5), i16(22), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(-5), i16(-12), i16(7), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), core::limitMax<i16>(), i16(0), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>() + 1), i16(1), i16(core::limitMin<i16>()), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>()), i16(core::limitMin<i16>()), i16(0), true },

    // I16 wrap around
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(-1), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(-core::limitMax<i16>()), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>()), i16(1), i16(0), false },

    // U16 no wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(0), u16(0), u16(0), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(13), u16(10), u16(3), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMin<u16>() + 1), u16(1), u16(core::limitMin<u16>()), true },

    // U16 wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMin<u16>()), u16(1), u16(0), false },

    // I32 no wrap around
    { __Wrapping_Math_TestCase::Type::I32, i32(0), i32(0), i32(0), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(11), i32(5), i32(6), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(17), i32(-5), i32(22), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(-5), i32(-12), i32(7), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), core::limitMax<i32>(), i32(0), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>() + 1), i32(1), i32(core::limitMin<i32>()), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>()), i32(core::limitMin<i32>()), i32(0), true },

    // I32 wrap around
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(-1), i32(0), false },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(-core::limitMax<i32>()), i32(0), false },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>()), i32(1), i32(0), false },

    // U32 no wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(0), u32(0), u32(0), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(13), u32(10), u32(3), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMin<u32>() + 1), u32(1), u32(core::limitMin<u32>()), true },

    // U32 wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMin<u32>()), u32(1), u32(0), false },

    // I64 no wrap around
    { __Wrapping_Math_TestCase::Type::I64, i64(0), i64(0), i64(0), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(11), i64(5), i64(6), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(17), i64(-5), i64(22), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-5), i64(-12), i64(7), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), core::limitMax<i64>(), i64(0), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>() + 1), i64(1), i64(core::limitMin<i64>()), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>()), i64(core::limitMin<i64>()), i64(0), true },

    // I64 wrap around
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(-1), i64(0), false },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(-core::limitMax<i64>()), i64(0), false },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>()), i64(1), i64(0), false },

    // U64 no wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(0), u64(0), u64(0), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(13), u64(10), u64(3), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMin<u64>() + 1), u64(1), u64(core::limitMin<u64>()), true },

    // U64 wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMin<u64>()), u64(1), u64(0), false },
};

constexpr __Wrapping_Math_TestCase __wrapping_mul_testCases[] = {
    // I8 no wrap around
    { __Wrapping_Math_TestCase::Type::I8, i8(8), i8(4), i8(32), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(8), i8(-2), i8(-16), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-5), i8(-12), i8(60), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-8), i8(-15), i8(120), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-7), i8(15), i8(-105), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(21), i8(6), i8(126), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(8), i8(-16), i8(core::limitMin<i8>()), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), i8(1), i8(core::limitMax<i8>()), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), i8(-1), i8(-127), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>()), i8(1), i8(core::limitMin<i8>()), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>() / 2), i8(2), i8(126), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(0), i8(-5), i8(0), true },
    { __Wrapping_Math_TestCase::Type::I8, i8(-3), i8(4), i8(-12), true },

    // I8 wrap around
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>()), i8(-1), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, i8(-1), i8(core::limitMin<i8>()), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMin<i8>()), i8(core::limitMin<i8>()), i8(0), false },
    { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>()), i8(2), i8(0), false },

    // U8 no wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(0), u8(0), u8(0), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(7), u8(5), u8(35), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(8), u8(4), u8(32), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(127), u8(2), u8(254), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(85), u8(3), u8(core::limitMax<u8>()), true }, // Max value without wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(1), u8(core::limitMax<u8>()), true },
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMin<u8>()), u8(1), u8(core::limitMin<u8>()), true }, // Multiplication with 0
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>() / 2), u8(2), u8(core::limitMax<u8>() - 1), true },

    // U8 wrap around
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(2), u8(0), false },
    { __Wrapping_Math_TestCase::Type::U8, u8(16), u8(16), u8(0), false },
    { __Wrapping_Math_TestCase::Type::U8, u8(200), u8(2), u8(144), false },
    { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(core::limitMax<u8>()), u8(1), false },

    // I16 no wrap around
    { __Wrapping_Math_TestCase::Type::I16, i16(0), i16(0), i16(0), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(7), i16(5), i16(35), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(8), i16(4), i16(32), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(8), i16(-2), i16(-16), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(-5), i16(-12), i16(60), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(-8), i16(-15), i16(120), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(-7), i16(15), i16(-105), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(21), i16(6), i16(126), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>() / 8), i16(8), i16(core::limitMin<i16>()), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(1), i16(core::limitMax<i16>()), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(-1), i16(core::limitMin<i16>() + 1), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>()), i16(1), i16(core::limitMin<i16>()), true },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>() / 2), i16(2), i16(core::limitMax<i16>() - 1), true },

    // I16 wrap around
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(2), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>()), i16(-2), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>() / 128), i16(256), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>() / 128), i16(-256), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMin<i16>()), i16(-1), i16(0), false },
    { __Wrapping_Math_TestCase::Type::I16, i16(-1), i16(core::limitMin<i16>()), i16(0), false },

    // U16 no wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(0), u16(0), u16(0), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(7), u16(5), u16(35), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(8), u16(4), u16(32), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(1), u16(core::limitMax<u16>()), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMin<u16>()), u16(1), u16(core::limitMin<u16>()), true },
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>() / 2), u16(2), u16(core::limitMax<u16>() - 1), true },

    // U16 wrap around
    { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(2), u16(0), false }, // Overflow with max * 2
    { __Wrapping_Math_TestCase::Type::U16, u16(40000), u16(2), u16(14464), false }, // Overflow with large values
    { __Wrapping_Math_TestCase::Type::U16, u16(30000), u16(3), u16(24608), false }, // Another overflow case

    // I32 no wrap around
    { __Wrapping_Math_TestCase::Type::I32, i32(0), i32(0), i32(0), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(7), i32(5), i32(35), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(8), i32(4), i32(32), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(7), i32(-1), i32(-7), true }, // Positive * Negative
    { __Wrapping_Math_TestCase::Type::I32, i32(-19), i32(-1), i32(19), true }, // Negative * Negative
    { __Wrapping_Math_TestCase::Type::I32, i32(8), i32(-2), i32(-16), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(-5), i32(-12), i32(60), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(-8), i32(-15), i32(120), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(-7), i32(15), i32(-105), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(21), i32(6), i32(126), true },
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>() / 8), i32(8), i32(core::limitMin<i32>()), true }, // Min value edge case
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>() / 1024), i32(1024), i32(core::limitMin<i32>()), true }, // Edge multiplication
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(1), i32(core::limitMax<i32>()), true }, // Max * 1
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(-1), i32(core::limitMin<i32>() + 1), true }, // Max * -1
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>()), i32(1), i32(core::limitMin<i32>()), true }, // Min * 1
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>() / 2), i32(2), i32(core::limitMax<i32>() - 1), true }, // Max/2 * 2

    // I32 wrap around
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(2), i32(0), false }, // Max * 2 causes overflow
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMax<i32>()), i32(-2), i32(0), false }, // Max * -2 overflow
    { __Wrapping_Math_TestCase::Type::I32, i32(core::limitMin<i32>()), i32(-1), i32(0), false }, // Min * -1 causes overflow
    { __Wrapping_Math_TestCase::Type::I32, i32(-1), i32(core::limitMin<i32>()), i32(0), false }, // -1 * Min  causes overflow
    { __Wrapping_Math_TestCase::Type::I32, i32(300000), i32(10000), i32(0), false }, // Large positive overflow
    { __Wrapping_Math_TestCase::Type::I32, i32(-300000), i32(10000), i32(0), false }, // Large negative overflow

    // U32 no wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(0), u32(0), u32(0), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(7), u32(5), u32(35), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(8), u32(4), u32(32), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(1), u32(core::limitMax<u32>()), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMin<u32>()), u32(1), u32(core::limitMin<u32>()), true },
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>() / 2), u32(2), u32(core::limitMax<u32>() - 1), true },

    // U32 wrap around
    { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(2), u32(0), false }, // Max * 2 causes overflow
    { __Wrapping_Math_TestCase::Type::U32, u32(3000000000), u32(3), u32(699999808), false }, // Overflow with large values
    { __Wrapping_Math_TestCase::Type::U32, u32(4000000000), u32(2), u32(3705032704), false }, // Another large value overflow

    // I64 no wrap around
    { __Wrapping_Math_TestCase::Type::I64, i64(0), i64(0), i64(0), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(7), i64(5), i64(35), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(8), i64(4), i64(32), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(7), i64(-1), i64(-7), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-19), i64(-1), i64(19), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(8), i64(-2), i64(-16), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-5), i64(-12), i64(60), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-8), i64(-15), i64(120), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(-7), i64(15), i64(-105), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(21), i64(6), i64(126), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>() / 8), i64(8), i64(core::limitMin<i64>()), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>() / 1024), i64(1024), i64(core::limitMin<i64>()), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(1), i64(core::limitMax<i64>()), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(-1), i64(core::limitMin<i64>() + 1), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>()), i64(1), i64(core::limitMin<i64>()), true },
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>() / 2), i64(2), i64(core::limitMax<i64>() - 1), true },

    // I64 wrap around
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(2), i64(0), false }, // Max * 2 causes overflow
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMax<i64>()), i64(-2), i64(0), false }, // Max * -2 causes overflow
    { __Wrapping_Math_TestCase::Type::I64, i64(core::limitMin<i64>()), i64(-1), i64(0), false }, // Min * -1 causes overflow
    { __Wrapping_Math_TestCase::Type::I64, i64(-1), i64(core::limitMin<i64>()), i64(0), false }, // -1 * Min  causes overflow
    { __Wrapping_Math_TestCase::Type::I64, i64(922337203685477580LL), i64(100), i64(0), false }, // Large overflow
    { __Wrapping_Math_TestCase::Type::I64, i64(-922337203685477580LL), i64(100), i64(0), false }, // Another large overflow

    // U64 no wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(0), u64(0), u64(0), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(7), u64(5), u64(35), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(8), u64(4), u64(32), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(1), u64(core::limitMax<u64>()), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMin<u64>()), u64(1), u64(core::limitMin<u64>()), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>() / 2), u64(2), u64(core::limitMax<u64>() - 1), true },
    { __Wrapping_Math_TestCase::Type::U64, u64(5000000000000000000), u64(2), u64(10000000000000000000ULL), true },

    // U64 wrap around
    { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(2), u64(0), false }, // Max * 2 causes overflow
    { __Wrapping_Math_TestCase::Type::U64, u64(10000000000000000000ULL), u64(2), u64(15519222885696424448ULL), false }, // Large overflow
    { __Wrapping_Math_TestCase::Type::U64, u64(9223372036854775808ULL), u64(2), u64(0), false }, // Another large overflow
};
