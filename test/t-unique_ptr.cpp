template <typename TAllocator>
i32 initialize_unique_ptr() {
    {
        core::unique_ptr<i32, TAllocator> p;
        Assert(p.get() == nullptr);
        p = core::make_unique<i32, TAllocator>(42);
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        core::unique_ptr<i32, TAllocator> p(core::make_unique<i32, TAllocator>(42));
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        core::unique_ptr<i32, TAllocator> p = core::make_unique<i32, TAllocator>(42);
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        auto p1 = core::make_unique<i32, TAllocator>(42);
        auto p2 = core::make_unique<i32, TAllocator>(24);
        Assert(p1.get() != nullptr);
        Assert(p2.get() != nullptr);
        Assert(*p1.get() == 42);
        Assert(*p2.get() == 24);
        Assert(*p1 == 42);
        Assert(*p2 == 24);
        
        p1 = core::move(p2);
        Assert(p1.get() != nullptr);
        Assert(p2.get() == nullptr);
        Assert(*p1.get() == 24);
        Assert(*p1 == 24);
    }
    {
        core::expected<core::unique_ptr<i32, TAllocator>, i32> p = core::make_unique<i32, TAllocator>(42);
        Assert(p.has_value());
        Assert(p.value().get() != nullptr);
        Assert(*p.value().get() == 42);
    }
    {
        // FIXME: Comprehensive constructor/destructor tests here...
    }
    
    return 0;
}

// FIXME: Test other functions of the unique pointer...