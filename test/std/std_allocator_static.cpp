struct std_allocator_static {
    static core::std_stats_allocator stdAlloc;

    static void* alloc(addr_size size) noexcept {
        return stdAlloc.alloc(size);
    }

    static void* calloc(addr_size count, addr_size size) noexcept {
        return stdAlloc.calloc(count, size);
    }

    static void free(void* ptr) noexcept {
        stdAlloc.free(ptr);
    }

    static addr_size used_mem() noexcept {
        return stdAlloc.used_mem();
    }

    static const char* allocator_name() noexcept {
        return stdAlloc.allocator_name();
    }
};

core::std_stats_allocator std_allocator_static::stdAlloc;
