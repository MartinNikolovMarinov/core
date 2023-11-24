#include "index.h"

template <typename TAllocator>
i32 initializeUniquePtr() {
    {
        core::UniquePtr<i32, TAllocator> p;
        Assert(p.get() == nullptr);
        p = core::makeUnique<i32, TAllocator>(42);
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocator> p(core::makeUnique<i32, TAllocator>(42));
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
        Assert(p.get() != nullptr);
        Assert(*p.get() == 42);
        Assert(*p == 42);
    }
    {
        auto p1 = core::makeUnique<i32, TAllocator>(42);
        auto p2 = core::makeUnique<i32, TAllocator>(24);
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
        core::expected<core::UniquePtr<i32, TAllocator>, i32> p = core::makeUnique<i32, TAllocator>(42);
        Assert(p.hasValue());
        Assert(p.value().get() != nullptr);
        Assert(*p.value().get() == 42);
    }
    {
        defer { CT::resetAll(); };

        {
            core::UniquePtr<CT, TAllocator> p;
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
        }

        CT::resetAll();
        {
            auto p = core::makeUnique<CT, TAllocator>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
        }
        Assert(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT, TAllocator> p;
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::dtorsCalled() == 0);
        }
        Assert(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT, TAllocator> p;
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 0);
            Assert(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::totalCtorsCalled() == 2);
            Assert(CT::dtorsCalled() == 1);
        }
        Assert(CT::dtorsCalled() == 2);

        CT::resetAll();
        {
            CT* ct = TAllocator::template construct<CT>();
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);

            core::UniquePtr<CT, TAllocator> p(ct);
            // Do not call the type's constructor when creating a UniquePtr from a raw pointer.
            Assert(CT::totalCtorsCalled() == 1);
            Assert(CT::assignmentsTotalCalled() == 0);
            Assert(CT::dtorsCalled() == 0);

            core::UniquePtr<CT, TAllocator> p2;

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
i32 stealUniquePtr() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    i32* raw = p.steal();
    Assert(raw != nullptr);
    Assert(*raw == 42);
    Assert(p.get() == nullptr);
    TAllocator::free(raw);
    return 0;
}

template <typename TAllocator>
i32 resetUniquePtr() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    Assert(p.get() != nullptr);
    Assert(*p.get() == 42);
    Assert(*p == 42);
    p.reset();
    Assert(p.get() == nullptr);
    return 0;
}

template <typename TAllocator>
i32 copyUniquePtr() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    Assert(p.get() != nullptr);
    Assert(*p.get() == 42);
    Assert(*p == 42);
    core::UniquePtr<i32, TAllocator> p2 = p.copy();
    Assert(p2.get() != nullptr);
    Assert(*p2.get() == 42);
    Assert(*p2 == 42);
    Assert(p.get() != p2.get());
    return 0;
}

template <typename TAllocator>
i32 swapUniquePtr() {
    core::UniquePtr<i32, TAllocator> p1 = core::makeUnique<i32, TAllocator>(42);
    core::UniquePtr<i32, TAllocator> p2 = core::makeUnique<i32, TAllocator>(24);
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

i32 runUniquePtrTestsSuite() {

    constexpr addr_size BUFF_SIZE = core::KILOBYTE;
    char buf[BUFF_SIZE];

    core::StdAllocator::init(nullptr);
    core::StdStatsAllocator::init(nullptr);
    core::BumpAllocator::init(nullptr, buf, BUFF_SIZE);

    auto checkLeaks = []() {
        Assert(core::StdAllocator::usedMem() == 0);
        Assert(core::StdStatsAllocator::usedMem() == 0, "Memory leak detected!");
        Assert(core::BumpAllocator::usedMem() == 0);
    };

    {
        RunTest(initializeUniquePtr<core::StdAllocator>);
        RunTest(initializeUniquePtr<core::StdStatsAllocator>);
        RunTest(initializeUniquePtr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(stealUniquePtr<core::StdAllocator>);
        RunTest(stealUniquePtr<core::StdStatsAllocator>);
        RunTest(stealUniquePtr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(resetUniquePtr<core::StdAllocator>);
        RunTest(resetUniquePtr<core::StdStatsAllocator>);
        RunTest(resetUniquePtr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(copyUniquePtr<core::StdAllocator>);
        RunTest(copyUniquePtr<core::StdStatsAllocator>);
        RunTest(copyUniquePtr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest(swapUniquePtr<core::StdAllocator>);
        RunTest(swapUniquePtr<core::StdStatsAllocator>);
        RunTest(swapUniquePtr<core::BumpAllocator>);
        core::BumpAllocator::clear();
        checkLeaks();
    }

    return 0;
}
