#pragma once

// FIXME: This is very far from elegant... :)

struct std_allocator_static;
#define CORE_DEFAULT_ALLOCATOR() std_allocator_static

#include <core.h>
#include <std/core.h>

using namespace coretypes;

struct std_allocator_static {
    static core::std_stats_allocator stdAlloc;

    static void* alloc(ptr_size size) noexcept;

    static void free(void* ptr) noexcept;

    static ptr_size used_mem() noexcept;

    static const char* allocator_name() noexcept;
};

core::std_stats_allocator std_allocator_static::stdAlloc;

void init_core();
