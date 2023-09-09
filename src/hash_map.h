#pragma once

#include <types.h>
#include <alloc.h>
#include <mem.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

template<typename T, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct arr {
    using data_type      = T;
    using size_type      = addr_size;
    using allocator_type = TAllocator;

    static constexpr bool dataIsTrivial = core::is_trivial_v<data_type>;
    static constexpr bool dataHasTrivialDestructor = core::is_trivially_destructible_v<data_type>;


};

static_assert(core::is_standard_layout_v<arr<i32>>, "arr<i32> must be standard layout");

} // namespace core
