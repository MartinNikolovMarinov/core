#pragma once

#include "example_interface_file.h"
#include "example_impl1.h"

namespace core
{

struct MyAllocator2 {
    MyAllocator a1;
};

template<>
void* Alloc<MyAllocator2>(MyAllocator2& allocator, ptr_size size) {
    return (void*)(allocator.a1.state - size);
}

}
