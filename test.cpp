#include <core.h>
#include <std/core.h>

#include <iostream>

using namespace coretypes;

static i32 g_testCount = 0;

#define RunTest(test, ...)                                                                      \
    g_testCount++;                                                                              \
    std::cout << "\t[TEST " << "№ " << g_testCount << " RUNNING] " << ANSI_BOLD(#test) << '\n'; \
    { [[maybe_unused]] auto __notused__ = test(__VA_ARGS__); }                                  \
    std::cout << "\t[TEST " << "№ " << g_testCount << ANSI_BOLD(ANSI_GREEN(" PASSED")) << "] " << ANSI_BOLD(#test) << std::endl;

#if defined(RUN_COMPILETIME_TESTS) && RUN_COMPILETIME_TESTS == 1
    #define RunTestCompileTime(test, ...) \
        { [[maybe_unused]] constexpr auto __notused__ = core::force_consteval<test(__VA_ARGS__)>; }
#else
    #define RunTestCompileTime(...)
    #define RUN_COMPILETIME_TESTS 0
#endif


#define RunTestSuite(suite, ...)                                       \
    std::cout << "[SUITE RUNNING] " << ANSI_BOLD(#suite) << std::endl; \
    suite(__VA_ARGS__);                                                \
    std::cout << "[SUITE " << ANSI_BOLD(ANSI_GREEN("PASSED")) << "] " << ANSI_BOLD(#suite) << std::endl;

#include "./common_test_helpers.h"
#include "test/run_tests.cpp"
#include "test/std/run_tests.cpp"

// i32 main(i32, const char **) {
//     core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
//         constexpr u32 stackFramesToSkip = 3;
//         std::string trace = core::stacktrace(200, stackFramesToSkip);
//         std::cout << ANSI_RED_START() << ANSI_BOLD_START()
//                   << "[ASSERTION] [EXPR]: " << failedExpr
//                   << " [FILE]: " << file
//                   << " [LINE]: " << line
//                   << " [MSG]: " << errMsg
//                   << ANSI_RESET()
//                   << std::endl;
//         std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;
//         throw std::runtime_error("Assertion failed!");
//         return false;
//     });

//     // Print compiler
//     if constexpr (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
//     if constexpr (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
//     if constexpr (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
//     if constexpr (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

//     // Print OS
//     if constexpr (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
//     if constexpr (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
//     if constexpr (OS_MAC == 1)     { std::cout << "[OS] OS_MAC" << std::endl; }
//     if constexpr (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

//     i32 exitCode = 0;

//     std::cout << "\n" << "RUNNING COMMON TESTS" << "\n\n";
//     exitCode += run_all_tests();
//     std::cout << "\n" << "RUNNING STD TESTS" << "\n\n";
//     exitCode += run_all_std_tests();

//     std::cout << '\n';
//     std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;

//     if constexpr (RUN_COMPILETIME_TESTS != 1) {
//         std::cout << ANSI_YELLOW_START() << ANSI_BOLD_START()
//                   << "[WARN] DID NOT RUN COMPILETIME TESTS!"
//                   << ANSI_RESET() << std::endl;
//     }

//     return exitCode;
// }

template<>
addr_size core::hash(const i32& key) {
    addr_size h = addr_size(core::simple_hash_32(reinterpret_cast<const void*>(&key), sizeof(key)));
    return h;
}

template<>
bool core::eq(const i32& a, const i32& b) {
    return a == b;
}

template<>
addr_size core::hash(const core::str_view& key) {
    addr_size h = addr_size(core::simple_hash_32(key.data(), key.len));
    return h;
}

template<>
bool core::eq(const core::str_view& a, const core::str_view& b) {
    return a == b;
}

i32 main(i32, const char**) {
    {
        core::hash_map<i32, i32, std_allocator_static> m;

        auto printKeys = [](auto& m) {
            std::cout << "Keys:" << std::endl;
            m.keys([](const i32& key) {
                std::cout << key << ", ";
            });
            std::cout << std::endl;
        };

        auto printValues = [](auto& m) {
            std::cout << "Values:" << std::endl;
            m.values([](const i32& value) {
                std::cout << value << ", ";
            });
            std::cout << std::endl;
        };

        m.put(1, 1);
        m.put(2, 1);
        m.put(3, 1);
        m.put(1, 5);

        auto a0 = m.get(1);
        auto a1 = m.get(2);
        auto a2 = m.get(3);
        auto a3 = m.get(4);

        std::cout << *a0 << std::endl;
        std::cout << *a1 << std::endl;
        std::cout << *a2 << std::endl;
        std::cout << a3 << std::endl;

        printKeys(m);
        printValues(m);
    }

    {
        core::hash_map<core::str_view, i32, std_allocator_static> m;
        using core::sv;

        auto printKeys = [](auto& m) {
            std::cout << "Keys:" << std::endl;
            m.keys([](const core::str_view& key) {
                std::cout << key.buff << ", ";
            });
            std::cout << std::endl;
        };

        auto printValues = [](auto& m) {
            std::cout << "Values:" << std::endl;
            m.values([](const i32& value) {
                std::cout << value << ", ";
            });
            std::cout << std::endl;
        };

        m.put(sv("1"), 1);
        m.put(sv("2"), 1);
        m.put(sv("3"), 1);
        m.put(sv("1"), 5);

        auto a0 = m.get(sv("1"));
        auto a1 = m.get(sv("2"));
        auto a2 = m.get(sv("3"));
        auto a3 = m.get(sv("4"));

        std::cout << *a0 << std::endl;
        std::cout << *a1 << std::endl;
        std::cout << *a2 << std::endl;
        std::cout << a3 << std::endl;

        printKeys(m);
        printValues(m);
    }

    return 0;
}
