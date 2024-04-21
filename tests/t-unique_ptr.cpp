#include "t-index.h"

template <typename TAllocator>
i32 initializeUniquePtrTest() {
    using CT = core::testing::CT;

    {
        core::UniquePtr<i32, TAllocator> p;
        CT_CHECK(p.get() == nullptr);
        p = core::makeUnique<i32, TAllocator>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocator> p(core::makeUnique<i32, TAllocator>(42));
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        auto p1 = core::makeUnique<i32, TAllocator>(42);
        auto p2 = core::makeUnique<i32, TAllocator>(24);
        CT_CHECK(p1.get() != nullptr);
        CT_CHECK(p2.get() != nullptr);
        CT_CHECK(*p1.get() == 42);
        CT_CHECK(*p2.get() == 24);
        CT_CHECK(*p1 == 42);
        CT_CHECK(*p2 == 24);

        p1 = std::move(p2);
        CT_CHECK(p1.get() != nullptr);
        CT_CHECK(p2.get() == nullptr);
        CT_CHECK(*p1.get() == 24);
        CT_CHECK(*p1 == 24);
    }
    {
        core::expected<core::UniquePtr<i32, TAllocator>, i32> p = core::makeUnique<i32, TAllocator>(42);
        CT_CHECK(p.hasValue());
        CT_CHECK(p.value().get() != nullptr);
        CT_CHECK(*p.value().get() == 42);
    }
    {
        defer { CT::resetAll(); };

        {
            core::UniquePtr<CT, TAllocator> p;
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
        }

        CT::resetAll();
        {
            auto p = core::makeUnique<CT, TAllocator>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT, TAllocator> p;
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT, TAllocator> p;
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT, TAllocator>();
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 1);
        }
        CT_CHECK(CT::dtorsCalled() == 2);

        CT::resetAll();
        {
            CT* ct = TAllocator::template construct<CT>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);

            core::UniquePtr<CT, TAllocator> p(ct);
            // Do not call the type's constructor when creating a UniquePtr from a raw pointer.
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);

            core::UniquePtr<CT, TAllocator> p2;

            p2 = std::move(p);
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(p.get() == nullptr);
        }
        CT_CHECK(CT::dtorsCalled() == 1);
    }

    return 0;
}

template <typename TAllocator>
i32 stealUniquePtrTest() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    i32* raw = p.steal();
    CT_CHECK(raw != nullptr);
    CT_CHECK(*raw == 42);
    CT_CHECK(p.get() == nullptr);
    TAllocator::free(raw, sizeof(i32));
    return 0;
}

template <typename TAllocator>
i32 resetUniquePtrTest() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    p.reset();
    CT_CHECK(p.get() == nullptr);
    return 0;
}

template <typename TAllocator>
i32 copyUniquePtrTest() {
    core::UniquePtr<i32, TAllocator> p = core::makeUnique<i32, TAllocator>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    core::UniquePtr<i32, TAllocator> p2 = p.copy();
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p2.get() == 42);
    CT_CHECK(*p2 == 42);
    CT_CHECK(p.get() != p2.get());
    return 0;
}

template <typename TAllocator>
i32 swapUniquePtrTest() {
    core::UniquePtr<i32, TAllocator> p1 = core::makeUnique<i32, TAllocator>(42);
    core::UniquePtr<i32, TAllocator> p2 = core::makeUnique<i32, TAllocator>(24);
    CT_CHECK(p1.get() != nullptr);
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p1.get() == 42);
    CT_CHECK(*p2.get() == 24);
    CT_CHECK(*p1 == 42);
    CT_CHECK(*p2 == 24);

    p1.swap(p2);
    CT_CHECK(p1.get() != nullptr);
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p1.get() == 24);
    CT_CHECK(*p2.get() == 42);
    CT_CHECK(*p1 == 24);
    CT_CHECK(*p2 == 42);
    return 0;
}

i32 runUniquePtrTestsSuite() {

    constexpr addr_size BUFF_SIZE = core::KILOBYTE;
    char buf[BUFF_SIZE];

    core::StdAllocator::init(nullptr);
    core::StdStatsAllocator::init(nullptr);
    core::BumpAllocator::init(nullptr, buf, BUFF_SIZE);

    auto checkLeaks = []() {
        CT_CHECK(core::StdAllocator::usedMem() == 0);
        CT_CHECK(core::StdStatsAllocator::usedMem() == 0, "Memory leak detected!");
        CT_CHECK(core::BumpAllocator::usedMem() == 0);
    };

    {
        RunTest_DisplayMemAllocs(initializeUniquePtrTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(initializeUniquePtrTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(initializeUniquePtrTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(stealUniquePtrTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(stealUniquePtrTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(stealUniquePtrTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(resetUniquePtrTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(resetUniquePtrTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(resetUniquePtrTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(copyUniquePtrTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(copyUniquePtrTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(copyUniquePtrTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }
    {
        RunTest_DisplayMemAllocs(swapUniquePtrTest<core::StdAllocator>, core::StdAllocator);
        RunTest_DisplayMemAllocs(swapUniquePtrTest<core::StdStatsAllocator>, core::StdStatsAllocator);
        RunTest_DisplayMemAllocs(swapUniquePtrTest<core::BumpAllocator>, core::BumpAllocator);
        core::BumpAllocator::clear();
        checkLeaks();
    }

    return 0;
}
