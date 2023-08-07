#pragma once

// IMPORTANT: This file initializes default settings for core and std core. Anywhere the application wants to use
// somthing from core or std core, it must include this file instead of directly including any specific header. This is
// because default macros need to be overridden before including any core or std core headers.

#include <std/allocators/alloc_std_stats.h>

using namespace coretypes;

static core::std_stats_allocator g_stdAlloc;
struct std_allocator_static {
    static void* alloc(addr_size size) noexcept;

    static void free(void* ptr) noexcept;

    static addr_size used_mem() noexcept;

    static const char* allocator_name() noexcept;
};

#undef CORE_DEFAULT_ALLOCATOR
#define CORE_DEFAULT_ALLOCATOR() std_allocator_static

#include <core.h>
#include <std/core.h>

void initCore(i32 argc, const char** argv);

struct command_line_args {
    i32 exampleId = -1;
    static constexpr i32 g_exampleCount = 5;
};

extern command_line_args g_cmdLineArgs;
