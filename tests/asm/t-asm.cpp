#include "../t-index.h"
#include "../common/wrapping_math_test_tables.h"

#if CPU_ARCH_X86_64

i32 asmAddWithWrappingCheck_x86_64() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_add_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::x86_asm_add_no_overflow(a, b, res);

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

i32 asmSubWithWrappingCheck_x86_64() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_sub_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::x86_asm_sub_no_overflow(a, b, res); // Signed overflow check

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

i32 asmMulWithWrappingCheck_x86_64() {
    using Type = __Wrapping_Math_TestCase::Type;

    i32 ret = core::testing::executeTestTable("test case failed at index: ", __wrapping_mul_testCases, [](auto& c, const char* cErr) {
        // A utility to handle the generic pattern
        auto runTestCase = [](auto a, auto b, auto want, bool cwrapAround, const char* ccErr) -> i32 {
            decltype(a) res = 0;
            bool noWrapAround = core::x86_asm_mul_overflow(a, b, res); // Signed overflow check

            // Check if the detected wraparound matches expected behavior
            CT_CHECK(noWrapAround == cwrapAround, ccErr);
            if (noWrapAround) CT_CHECK(res == want, ccErr);

            return 0;
        };

        switch (c.type)
        {
            case Type::I8:
                return runTestCase(c.a.as_i8, c.b.as_i8, c.want.as_i8, c.noWrapAround, cErr);
            case Type::I16:
                return runTestCase(c.a.as_i16, c.b.as_i16, c.want.as_i16, c.noWrapAround, cErr);
            case Type::I32:
                return runTestCase(c.a.as_i32, c.b.as_i32, c.want.as_i32, c.noWrapAround, cErr);
            case Type::I64:
                return runTestCase(c.a.as_i64, c.b.as_i64, c.want.as_i64, c.noWrapAround, cErr);
            case Type::U8:
                return runTestCase(c.a.as_u8, c.b.as_u8, c.want.as_u8, c.noWrapAround, cErr);
            case Type::U16:
                return runTestCase(c.a.as_u16, c.b.as_u16, c.want.as_u16, c.noWrapAround, cErr);
            case Type::U32:
                return runTestCase(c.a.as_u32, c.b.as_u32, c.want.as_u32, c.noWrapAround, cErr);
            case Type::U64:
                return runTestCase(c.a.as_u64, c.b.as_u64, c.want.as_u64, c.noWrapAround, cErr);
        }

        Panic(false, "unreachable");
        return 0;
    });

    return ret;
}

#endif

i32 runAsmTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

#if CPU_ARCH_X86_64

    tInfo.name = FN_NAME_TO_CPTR(asmAddWithWrappingCheck_x86_64);
    if (runTest(tInfo, asmAddWithWrappingCheck_x86_64) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(asmSubWithWrappingCheck_x86_64);
    if (runTest(tInfo, asmSubWithWrappingCheck_x86_64) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(asmMulWithWrappingCheck_x86_64);
    if (runTest(tInfo, asmMulWithWrappingCheck_x86_64) != 0) { ret = -1; }

    // FIXME: Write division tests.

#endif

    return ret;
}
