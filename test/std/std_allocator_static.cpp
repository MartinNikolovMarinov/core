// FIXME: use std allocator that counts allocations and frees to make sure tested data structures are freed properly!

struct std_allocator_static {
    static core::std_allocator stdAlloc;

    static void* alloc(ptr_size size) noexcept {
        return stdAlloc.alloc(size);
    }

    static void free(void* ptr) noexcept {
        stdAlloc.free(ptr);
    }

    static ptr_size used_mem() noexcept {
        return stdAlloc.used_mem();
    }

    static const char* allocator_name() noexcept {
        return stdAlloc.allocator_name();
    }
};

core::std_allocator std_allocator_static::stdAlloc;
