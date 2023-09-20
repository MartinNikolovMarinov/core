template<addr_size TCap>
struct bump_allocator_static {
    static core::bump_allocator<TCap> bumpAllocator;

    static void* alloc(addr_size size) noexcept {
        return bumpAllocator.alloc(size);
    }

    static void* calloc(addr_size count, addr_size size) noexcept {
        return bumpAllocator.calloc(count, size);
    }

    static void free(void* ptr) noexcept {
        bumpAllocator.free(ptr);
    }

    static addr_size used_mem() noexcept {
        return bumpAllocator.used_mem();
    }

    static void clear() noexcept {
        bumpAllocator.clear();
    }

    static const char* allocator_name() noexcept {
        return bumpAllocator.allocator_name();
    }
};

template<addr_size TCap>
core::bump_allocator<TCap> bump_allocator_static<TCap>::bumpAllocator;
