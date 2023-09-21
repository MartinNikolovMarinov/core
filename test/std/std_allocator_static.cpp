struct std_allocator_static {
    static core::std_stats_allocator stdStatsAlloc;

    static void* alloc(addr_size size) noexcept {
        return stdStatsAlloc.alloc(size);
    }

    static void* calloc(addr_size count, addr_size size) noexcept {
        return stdStatsAlloc.calloc(count, size);
    }

    static void free(void* ptr) noexcept {
        stdStatsAlloc.free(ptr);
    }

    static addr_size used_mem() noexcept {
        return stdStatsAlloc.used_mem();
    }

    static const char* allocator_name() noexcept {
        return stdStatsAlloc.allocator_name();
    }
};

core::std_stats_allocator std_allocator_static::stdStatsAlloc;
