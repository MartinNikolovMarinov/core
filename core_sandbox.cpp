#include "tests/t-index.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <array>
#include <bit>

void assertHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 2;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << (errMsg ? errMsg : "") // IMPORTANT: MSVC's std implementation will crash if errMsg is nullptr !
                << ANSI_RESET()
                << std::endl;
    std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;

    if (!ok) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "Failed to take full stacktrace. Consider resizing the stacktrace buffer size!"
                  << ANSI_RESET()
                  << std::endl;
    }

    throw std::runtime_error("Assertion failed!");
};

void testMemcopy();
void testMemset();
void testMemcmp();

i32 main() {
    core::initProgramCtx(assertHandler);

    testMemcopy();
    testMemset();
    testMemcmp();

    return 0;
}

void testMemcopy() {
    {
        const char* a = "abcd";

        char buffer1[256];
        addr_size n1 = core::cmemcopy(buffer1, a, core::cstrLen(a));
        buffer1[n1] = '\0';

        char buffer2[256];
        addr_size n2 = core::imemcopy(buffer2, a, core::cstrLen(a));
        buffer2[n2] = '\0';

        logInfo("buffer1: %s", buffer1);
        logInfo("buffer1 len: %llu", n1);

        logInfo("buffer2: %s", buffer2);
        logInfo("buffer2 len: %llu", n2);
    }

    {
        struct ComplicatedType {
            f32 a;
            i64 x;

            ComplicatedType() = default;
            ComplicatedType(f32 _a, i64 _x) : a(_a), x(_x) {}
        };
        static_assert(std::is_trivial_v<ComplicatedType>);

        const ComplicatedType a[4] = { {4.3f, 6}, {}, {}, {0.0003f, 9999} };

        ComplicatedType buffer1[4];
        addr_size n1 = core::cmemcopy(buffer1, a, CORE_C_ARRLEN(a));

        ComplicatedType buffer2[4];
        addr_size n2 = core::imemcopy(buffer2, a, CORE_C_ARRLEN(a));

        logInfo("buffer1 len: %llu", n1);
        logInfo("buffer2 len: %llu", n2);
    }
}

void testMemset() {
    {
        char buffer1[5] = {};
        addr_size n1 = core::cmemset(buffer1, 'a', CORE_C_ARRLEN(buffer1) - 1);
        buffer1[n1] = '\0';

        char buffer2[5];
        addr_size n2 = core::imemset(buffer2, 'b', CORE_C_ARRLEN(buffer2) - 1);
        buffer2[n2] = '\0';

        logInfo("buffer1: %s", buffer1);
        logInfo("buffer1 len: %llu", n1);

        logInfo("buffer2: %s", buffer2);
        logInfo("buffer2 len: %llu", n2);
    }

    {
        struct ComplicatedType {
            f32 a;
            i64 x;

            ComplicatedType() = default;
            ComplicatedType(f32 _a, i64 _x) : a(_a), x(_x) {}
        };
        static_assert(std::is_trivial_v<ComplicatedType>);

        ComplicatedType buffer1[5];
        addr_size n1 = core::cmemset(buffer1, ComplicatedType { 1.25f, 89 }, CORE_C_ARRLEN(buffer1));

        ComplicatedType buffer2[5];
        addr_size n2 = core::memset(buffer2, ComplicatedType { 0.99213f, 44 }, CORE_C_ARRLEN(buffer2));

        logInfo("buffer1 len: %llu", n1);
        logInfo("buffer2 len: %llu", n2);
    }
}

void testMemcmp() {
    {
        constexpr const char* a = "abcde";
        constexpr const char* b = "abcd";

        constexpr i32 res1 = core::cmemcmpBytes(a, core::cstrLen(a) - 2, b, core::cstrLen(b));
        i32 res11 = core::imemcmp(a, core::cstrLen(a) - 2, b, core::cstrLen(b));
        logInfo("res1: %d", res1);
        logInfo("res11: %d", res11);

        constexpr i32 res2 = core::cmemcmpBytes(a, core::cstrLen(a) - 1, b, core::cstrLen(b));
        i32 res22 = core::imemcmp(a, core::cstrLen(a) - 1, b, core::cstrLen(b));
        logInfo("res2: %d", res2);
        logInfo("res22: %d", res22);

        constexpr i32 res3 = core::cmemcmpBytes(a, core::cstrLen(a), b, core::cstrLen(b));
        i32 res33 = core::imemcmp(a, core::cstrLen(a), b, core::cstrLen(b));
        logInfo("res3: %d", res3);
        logInfo("res33: %d", res33);
    }

    {
        struct ComplicatedType {
            f32 a;
            i64 x;

            constexpr ComplicatedType() = default;
            constexpr ComplicatedType(f32 _a, i32 _x) : a(_a), x(_x) {}
        };
        static_assert(std::is_trivial_v<ComplicatedType>);

        constexpr auto cmpFn = [](const ComplicatedType& a, const ComplicatedType& b) -> i32 {
            if (a.a != b.a) return a.a > b.a ? 1 : -1;
            else if (a.x != b.x) return a.x > b.x ? 1 : -1;
            return 0;
        };

        constexpr ComplicatedType a[5] = { {1.1f, 60000}, {-0.3443f, 70000}, {0.f, 0}, {0.1f, 80000}, {9.99999f, 999999} };
        constexpr ComplicatedType b[4] = { {1.1f, 60000}, {-0.3443f, 70000}, {0.f, 0}, {0.1f, 80000} };

        constexpr i32 res1 = core::cmemcmp(a, CORE_C_ARRLEN(a) - 2, b, CORE_C_ARRLEN(b), cmpFn);
        i32 res11 = core::imemcmp(a, CORE_C_ARRLEN(a) - 2, b, CORE_C_ARRLEN(b));
        logInfo("res1: %d", res1);
        logInfo("res11: %d", res11);

        constexpr i32 res2 = core::cmemcmp(a, CORE_C_ARRLEN(a) - 1, b, CORE_C_ARRLEN(b), cmpFn);
        i32 res22 = core::imemcmp(a, CORE_C_ARRLEN(a) - 1, b, CORE_C_ARRLEN(b));
        logInfo("res2: %d", res2);
        logInfo("res22: %d", res22);

        constexpr i32 res3 = core::cmemcmp(a, CORE_C_ARRLEN(a), b, CORE_C_ARRLEN(b), cmpFn);
        i32 res33 = core::imemcmp(a, CORE_C_ARRLEN(a), b, CORE_C_ARRLEN(b));
        logInfo("res3: %d", res3);
        logInfo("res33: %d", res33);
    }
}

