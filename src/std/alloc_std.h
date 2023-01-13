#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT std_allocator {
    static constexpr const char* allocator_name() { return "standard library allocator"; }

    static void* alloc(ptr_size size) noexcept;

    static void free(void* ptr) noexcept;

    static ptr_size used_mem() noexcept;

    // FIXME: OOM handler:
    // using on_oom_fp = void(*)(void* userData);
    // static void on_out_of_memory(on_oom_fp cb) noexcept;
};

}
