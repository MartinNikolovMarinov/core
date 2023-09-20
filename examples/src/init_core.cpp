#include "init_core.h"

#include <fmt/format.h>
#include <fmt/color.h>

command_line_args g_cmdLineArgs;

void* std_allocator_static::alloc(addr_size size) noexcept {
    return g_stdAlloc.alloc(size);
}

void std_allocator_static::free(void* ptr) noexcept {
    g_stdAlloc.free(ptr);
}

addr_size std_allocator_static::used_mem() noexcept {
    return g_stdAlloc.used_mem();
}

const char* std_allocator_static::allocator_name() noexcept {
    return g_stdAlloc.allocator_name();
}

void printUsage() {
    fmt::print("Usage:\n");
    fmt::print("  -ex, --ex, --example\t\tChoose a specific example to run from 1 to {}\n", g_cmdLineArgs.g_exampleCount);
}

void initCore(i32 argc, const char** argv) {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        constexpr u32 stackFramesToSkip = 3;
        std::string trace = core::stacktrace(200, stackFramesToSkip);
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
                   "[ASSERTION] [EXPR]: {}\n[FILE]: {}:{}\n[MSG]: {}\n",
                    failedExpr, file, line, errMsg);
        fmt::print(fmt::emphasis::bold, "[TRACE]:\n{}\n", trace);
        throw std::runtime_error("Assertion failed!");
    });

    core::rnd_init();

    // Parse the command line arguments.
    if (argc > 1) {
        core::flag_parser parser;
        parser.allowUnknownFlags = true;
        parser.flag(&g_cmdLineArgs.exampleId, "ex", false, [](void* ex) -> bool {
            i32 v = *(i32*)ex;
            return v > 0 && v <= g_cmdLineArgs.g_exampleCount;
        });
        parser.flag(&g_cmdLineArgs.exampleId, "example", false, [](void* ex) -> bool {
            i32 v = *(i32*)ex;
            return v > 0 && v <= g_cmdLineArgs.g_exampleCount;
        });

        auto res = parser.parse(argc - 1, argv + 1);
        if (res.has_err()) {
            fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "Invalid command line argument.\n\n");
            printUsage();
            core::os_exit(-1);
        }
    }
}
