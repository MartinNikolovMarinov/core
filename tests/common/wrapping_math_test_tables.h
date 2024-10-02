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
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, i8(0), i8(0), i8(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, i8(5), i8(10), i8(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, i8(10), i8(-5), i8(5), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), core::limitMin<i8>(), i8(-1), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, i8(core::limitMax<i8>() - 1), i8(1), i8(core::limitMax<i8>()), true },

    // I8 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), i8(1), i8(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, core::limitMax<i8>(), core::limitMax<i8>(), i8(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, core::limitMin<i8>(), i8(-1), i8(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I8, core::limitMin<i8>(), core::limitMin<i8>(), i8(0), false },

    // U8 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U8, u8(0), u8(0), u8(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U8, u8(5), u8(10), u8(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>() - 1), u8(1), u8(core::limitMax<u8>()), true },

    // U8 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(1), u8(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U8, u8(core::limitMax<u8>()), u8(core::limitMax<u8>()), u8(0), false },

    // I16 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, i16(0), i16(0), i16(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, i16(5), i16(10), i16(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, i16(10), i16(-5), i16(5), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), core::limitMin<i16>(), i16(-1), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, i16(core::limitMax<i16>() - 1), i16(1), i16(core::limitMax<i16>()), true },

    // I16 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), i16(1), i16(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, core::limitMax<i16>(), core::limitMax<i16>(), i16(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, core::limitMin<i16>(), i16(-1), i16(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I16, core::limitMin<i16>(), core::limitMin<i16>(), i16(0), false },

     // U16 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U16, u16(0), u16(0), u16(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U16, u16(5), u16(10), u16(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>() - 1), u16(1), u16(core::limitMax<u16>()), true },

    // U16 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(1), u16(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U16, u16(core::limitMax<u16>()), u16(core::limitMax<u16>()), u16(0), false },

    // I32 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, i32(0), i32(0), i32(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, i32(5), i32(10), i32(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, i32(10), i32(-5), i32(5), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), core::limitMin<i32>(), i32(-1), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>() - 1, i32(1), core::limitMax<i32>(), true },

    // I32 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), i32(1), i32(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMax<i32>(), core::limitMax<i32>(), i32(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMin<i32>(), i32(-1), i32(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I32, core::limitMin<i32>(), core::limitMin<i32>(), i32(0), false },

    // U32 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U32, u32(0), u32(0), u32(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U32, u32(5), u32(10), u32(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>() - 1), u32(1), u32(core::limitMax<u32>()), true },

    // U32 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(1), u32(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U32, u32(core::limitMax<u32>()), u32(core::limitMax<u32>()), u32(0), false },

    // I64 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, i64(0), i64(0), i64(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, i64(5), i64(10), i64(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, i64(10), i64(-5), i64(5), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), core::limitMin<i64>(), i64(-1), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>() - 1, i64(1), core::limitMax<i64>(), true },

    // I64 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), i64(1), i64(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMax<i64>(), core::limitMax<i64>(), i64(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMin<i64>(), i64(-1), i64(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::I64, core::limitMin<i64>(), core::limitMin<i64>(), i64(0), false },

    // U64 no wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U64, u64(0), u64(0), u64(0), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U64, u64(5), u64(10), u64(15), true },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>() - 1), u64(1), u64(core::limitMax<u64>()), true },

    // U64 wrap around
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(1), u64(0), false },
    __Wrapping_Math_TestCase { __Wrapping_Math_TestCase::Type::U64, u64(core::limitMax<u64>()), u64(core::limitMax<u64>()), u64(0), false },
};
