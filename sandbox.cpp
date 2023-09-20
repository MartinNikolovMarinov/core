#include <core.h>
#include <std/core.h>

#include <iostream>

using namespace coretypes;

i32 main(i32, const char **) {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        constexpr u32 stackFramesToSkip = 3;
        std::string trace = core::stacktrace(200, stackFramesToSkip);
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "[ASSERTION] [EXPR]: " << failedExpr
                  << " [FILE]: " << file
                  << " [LINE]: " << line
                  << " [MSG]: " << errMsg
                  << ANSI_RESET()
                  << std::endl;
        std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;
        throw std::runtime_error("Assertion failed!");
        return false;
    });

    // Print compiler
    if constexpr (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
    if constexpr (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
    if constexpr (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
    if constexpr (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

    // Print OS
    if constexpr (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
    if constexpr (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
    if constexpr (OS_MAC == 1)     { std::cout << "[OS] OS_MAC" << std::endl; }
    if constexpr (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

    i32 exitCode = 0;

    // BEGIN SANDBOX CODE

    // END SANDBOX CODE

    return exitCode;
}