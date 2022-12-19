#pragma once

#include "example_interface_file.h"

namespace core
{

struct MyAllocator {
    i32 state = 17;
};

template<>
void* Alloc<MyAllocator>(MyAllocator& allocator, ptr_size size) {
    return (void*)(allocator.state + size);
}

}
