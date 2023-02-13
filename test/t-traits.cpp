constexpr void run_traits_tests() {
    static_assert(core::AlwaysFalse<i32> == false);
    static_assert(core::AlwaysFalse<void> == false);
    static_assert(core::AlwaysFalse<f32> == false);

    static_assert(core::TrueType_v == true);
    static_assert(core::FalseType_v == false);

    static_assert(core::IsLValue_v<i32> == false);
    static_assert(core::IsLValue_v<i32&> == true);
    static_assert(core::IsLValue_v<i32&&> == false);
    static_assert(core::IsLValue_v<i32*> == false);

    static_assert(core::IsRValue_v<i32> == false);
    static_assert(core::IsRValue_v<i32&> == false);
    static_assert(core::IsRValue_v<i32&&> == true);
    static_assert(core::IsRValue_v<i32*> == false);

    // Remove l/r value reference:
    {
        i32 v = 0;
        decltype(v)& lvalue = v;
        decltype(v)&& rvalue = 0;
        core::RemoveRef<decltype(lvalue)>::type t1 = 0;
        core::RemoveRef<decltype(rvalue)>::type t2 = 0;

        static_assert(core::IsLValue_v<decltype(lvalue)> == true);
        static_assert(core::IsLValue_v<decltype(rvalue)> == false);
        static_assert(core::IsLValue_v<decltype(t1)> == false);
        static_assert(core::IsLValue_v<decltype(t2)> == false);

        static_assert(core::IsRValue_v<decltype(lvalue)> == false);
        static_assert(core::IsRValue_v<decltype(rvalue)> == true);
        static_assert(core::IsRValue_v<decltype(t1)> == false);
        static_assert(core::IsRValue_v<decltype(t2)> == false);
    }

    // Add and remove l/r value references:
    {
        i32 v = 0;
        typename core::AddLValue<decltype(v)>::type lvalue = v;
        typename core::AddRValue<decltype(v)>::type rvalue = core::move(v);

        static_assert(core::IsLValue_v<decltype(lvalue)> == true);
        static_assert(core::IsRValue_v<decltype(rvalue)> == true);
    }

    // Is trivially destructable
    {
        struct A {};
        struct B { ~B() {} };
        struct C { C() = delete; };
        struct D { D() = default; };
        struct E { E() = default; ~E() = delete; };
        struct F { F() = delete; ~F() = delete; };
        struct G { G() = delete; ~G() = default; };
        struct H { i32 h; H(i32 a) : h(a) {} };
        struct I { virtual ~I() {} };
        struct J : I { ~J() override {} };
        struct K : I {};

        static_assert(core::IsTriviallyDestructible_v<A> == true);
        static_assert(core::IsTriviallyDestructible_v<B> == false);
        static_assert(core::IsTriviallyDestructible_v<C> == true);
        static_assert(core::IsTriviallyDestructible_v<D> == true);
        static_assert(core::IsTriviallyDestructible_v<E> == true);
        static_assert(core::IsTriviallyDestructible_v<F> == true);
        static_assert(core::IsTriviallyDestructible_v<G> == true);
        static_assert(core::IsTriviallyDestructible_v<H> == true);
        static_assert(core::IsTriviallyDestructible_v<I> == false);
        static_assert(core::IsTriviallyDestructible_v<J> == false);
        static_assert(core::IsTriviallyDestructible_v<K> == false);

        static_assert(core::IsTriviallyDestructible_v<void> == false); // why is void not trivially destructable ??
        static_assert(core::IsTriviallyDestructible_v<void*> == true);
        static_assert(core::IsTriviallyDestructible_v<bool (*)(const char*)> == true);
        static_assert(core::IsTriviallyDestructible_v<i32> == true);
        static_assert(core::IsTriviallyDestructible_v<i32&> == true);
        static_assert(core::IsTriviallyDestructible_v<i32&&> == true);
        static_assert(core::IsTriviallyDestructible_v<i32*> == true);
        static_assert(core::IsTriviallyDestructible_v<i32[10]> == true);
        static_assert(core::IsTriviallyDestructible_v<bool> == true);

        // This one is important, my array implemntation is not trivially destructable:
        static_assert(core::IsTriviallyDestructible_v<core::arr<i32>> == false);
    }
}

void run_traits_tests_suite() {
    RunTest(run_traits_tests);
}
