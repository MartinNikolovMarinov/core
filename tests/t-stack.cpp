#include "t-index.h"

namespace {

template <core::AllocatorId TAllocId>
i32 stackInitializationTest() {
    using namespace core::testing;

    {
        core::Stack<i32, TAllocId> stack;
        CT_CHECK(stack.len() == 0);
        CT_CHECK(stack.cap() == 0);
        CT_CHECK(stack.empty());
    }

    {
        core::Stack<i32, TAllocId> stack(8);
        CT_CHECK(stack.len() == 0);
        CT_CHECK(stack.cap() == 8);
        CT_CHECK(stack.empty());
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 stackPushPopTest() {
    using namespace core::testing;

    {
        core::Stack<i32, TAllocId> stack;

        for (i32 i = 0; i < 12; ++i) {
            stack.push(i);
            CT_CHECK(stack.len() == addr_size(i + 1));
            CT_CHECK(stack.top() == i);
        }

        for (i32 i = 11; i >= 0; --i) {
            i32 value = stack.pop();
            CT_CHECK(value == i);
            CT_CHECK(stack.len() == addr_size(i));
        }

        CT_CHECK(stack.empty());
    }

    {
        defer { CT::resetAll(); };

        core::Stack<CT, TAllocId> stack(2);

        CT ct{};
        CT::resetAll();

        stack.push(ct);
        stack.push(ct);

        CT_CHECK(CT::copyCtorCalled() == 2);
        CT_CHECK(CT::totalCtorsCalled() == 2);
        CT_CHECK(CT::dtorsCalled() == 0);

        CT::resetAll();

        {
            auto popped = stack.pop();
            CT_CHECK(popped.a == CT::defaultValue);
            CT_CHECK(CT::dtorsCalled() == 1);
        }

        CT_CHECK(stack.len() == 1);

        CT_CHECK(CT::moveCtorCalled() == 1);
        CT_CHECK(CT::totalCtorsCalled() == 1);
        CT_CHECK(CT::totalCtorsCalled() == 1);
    }

    return 0;
}

template <core::AllocatorId TAllocId>
i32 stackMoveSemanticsTest() {
    using namespace core::testing;

    {
        core::Stack<i32, TAllocId> stack;
        for (i32 i = 0; i < 5; ++i) {
            stack.push(i * 2);
        }

        core::Stack<i32, TAllocId> moved(std::move(stack));

        CT_CHECK(stack.len() == 0);
        CT_CHECK(stack.cap() == 0);
        CT_CHECK(stack.empty());

        CT_CHECK(moved.len() == 5);
        CT_CHECK(!moved.empty());
        CT_CHECK(moved.top() == 8);

        core::Stack<i32, TAllocId> assigned;
        assigned = std::move(moved);

        CT_CHECK(moved.len() == 0);
        CT_CHECK(moved.cap() == 0);
        CT_CHECK(assigned.len() == 5);
        CT_CHECK(assigned.top() == 8);
    }


    return 0;
}

template <core::AllocatorId TAllocId>
i32 stackEnsureCapacityTest() {
    using namespace core::testing;

    core::Stack<i32, TAllocId> stack(2);

    stack.push(1);
    stack.push(2);

    CT_CHECK(stack.cap() == 2);
    CT_CHECK(stack.len() == 2);
    CT_CHECK(stack.top() == 2);

    stack.ensureCap(10);

    CT_CHECK(stack.cap() == 10);
    CT_CHECK(stack.len() == 2);
    CT_CHECK(stack.top() == 2);

    stack.push(3);
    stack.push(4);

    CT_CHECK(stack.cap() == 10);
    CT_CHECK(stack.len() == 4);
    CT_CHECK(stack.top() == 4);

    return 0;
}

template <core::AllocatorId TAllocId>
i32 runStackTests(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo(RegisteredAllocators(TAllocId), sInfo.useAnsiColors, false);

    defer { core::getAllocator(TAllocId).clear(); };

    tInfo.name = FN_NAME_TO_CPTR(stackInitializationTest<TAllocId>);
    if (runTest(tInfo, stackInitializationTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(stackPushPopTest<TAllocId>);
    if (runTest(tInfo, stackPushPopTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(stackMoveSemanticsTest<TAllocId>);
    if (runTest(tInfo, stackMoveSemanticsTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(stackEnsureCapacityTest<TAllocId>);
    if (runTest(tInfo, stackEnsureCapacityTest<TAllocId>) != 0) { return -1; }

    return 0;
}

} // namespace

i32 runStackTestsSuite(const core::testing::TestSuiteInfo& sInfo) {
    using namespace core::testing;

    if (runStackTests<RA_STD_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runStackTests<RA_STD_STATS_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runStackTests<RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID>(sInfo) != 0) { return -1; }
    if (runStackTests<RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 3;
    char buf[BUFFER_SIZE];
    setBufferForBumpAllocator(buf, BUFFER_SIZE);
    if (runStackTests<RA_BUMP_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    setBlockSizeForArenaAllocator(256);
    if (runStackTests<RA_ARENA_ALLOCATOR_ID>(sInfo) != 0) { return -1; }

    return 0;
}

