// FIXME: If std::forward is implemented some of the allocators and the allocator interface
//       can be moved to the core part of the code, without requiring the application to link
//       the standard library.
//       This will be required if I want data structures which do not depend on std.

#include "alloc_interface.h"
#include "alloc_bump.h"
#include "alloc_std.h"
