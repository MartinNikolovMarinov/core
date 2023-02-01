#include "init_core.h"

#include <fmt/format.h>
#include <fmt/color.h>

void* std_allocator_static::alloc(ptr_size size) noexcept {
    return g_stdAlloc.alloc(size);
}

void std_allocator_static::free(void* ptr) noexcept {
    g_stdAlloc.free(ptr);
}

ptr_size std_allocator_static::used_mem() noexcept {
    return g_stdAlloc.used_mem();
}

const char* std_allocator_static::allocator_name() noexcept {
    return g_stdAlloc.allocator_name();
}

void init_core() {
    core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
                   "[ASSERTION] [EXPR]: {}\n[FILE]: {}\n[LINE]: {}\n[MSG]: {}\n",
                    failedExpr, file, line, errMsg);
        throw std::runtime_error("Assertion failed!");
        return false;
    });
}
