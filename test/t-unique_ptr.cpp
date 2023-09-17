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
        defer { CT::resetAll(); };

        {
            core::unique_ptr<CT, TAllocator> p;
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
        }
        
        CT::resetAll();
        {
            auto p = core::make_unique<CT, TAllocator>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
        }
        Assert(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::unique_ptr<CT, TAllocator> p;
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
            p = core::make_unique<CT, TAllocator>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::dtorsCalled() == 0);
        }
        Assert(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::unique_ptr<CT, TAllocator> p;
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
            p = core::make_unique<CT, TAllocator>();
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::dtorsCalled() == 0);
            p = core::make_unique<CT, TAllocator>();
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 2);
            Assert(CT::dtorsCalled() == 1);
        }
        Assert(CT::dtorsCalled() == 2);

        CT::resetAll();
        {
            CT* ct = core::construct<TAllocator, CT>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);

            core::unique_ptr<CT, TAllocator> p(ct);
            // Do not call the type's constructor when creating a unique_ptr from a raw pointer.
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);

            core::unique_ptr<CT, TAllocator> p2;

            p2 = core::move(p);
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
            Assert(p.get() == nullptr);
        }
        Assert(CT::dtorsCalled() == 1);
    }
    
    return 0;
}

template <typename TAllocator>
i32 steal_unique_ptr() {
    core::unique_ptr<i32, TAllocator> p = core::make_unique<i32, TAllocator>(42);
    i32* raw = p.steal();
    Assert(raw != nullptr);
    Assert(*raw == 42);
    Assert(p.get() == nullptr);
    core::free<TAllocator>(raw);
    return 0;
}

template <typename TAllocator>
i32 reset_unique_ptr() {
    core::unique_ptr<i32, TAllocator> p = core::make_unique<i32, TAllocator>(42);
    Assert(p.get() != nullptr);
    Assert(*p.get() == 42);
    Assert(*p == 42);
    p.reset();
    Assert(p.get() == nullptr);
    return 0;
}

template <typename TAllocator>
i32 copy_unique_ptr() {
    core::unique_ptr<i32, TAllocator> p = core::make_unique<i32, TAllocator>(42);
    Assert(p.get() != nullptr);
    Assert(*p.get() == 42);
    Assert(*p == 42);
    core::unique_ptr<i32, TAllocator> p2 = p.copy();
    Assert(p2.get() != nullptr);
    Assert(*p2.get() == 42);
    Assert(*p2 == 42);
    Assert(p.get() != p2.get());
    return 0;
}

template <typename TAllocator>
i32 swap_unique_ptr() {
    core::unique_ptr<i32, TAllocator> p1 = core::make_unique<i32, TAllocator>(42);
    core::unique_ptr<i32, TAllocator> p2 = core::make_unique<i32, TAllocator>(24);
    Assert(p1.get() != nullptr);
    Assert(p2.get() != nullptr);
    Assert(*p1.get() == 42);
    Assert(*p2.get() == 24);
    Assert(*p1 == 42);
    Assert(*p2 == 24);
    
    p1.swap(p2);
    Assert(p1.get() != nullptr);
    Assert(p2.get() != nullptr);
    Assert(*p1.get() == 24);
    Assert(*p2.get() == 42);
    Assert(*p1 == 24);
    Assert(*p2 == 42);
    return 0;
}