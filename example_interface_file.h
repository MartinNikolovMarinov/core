#pragma once

#include <types.h>

namespace core
{

using namespace coretypes;

template<class A> void* Alloc(A& allocator, ptr_size size);

}
