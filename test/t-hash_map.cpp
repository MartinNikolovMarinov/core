template<>
addr_size core::hash(const i32& key) {
    addr_size h = addr_size(core::simple_hash_32(reinterpret_cast<const void*>(&key), sizeof(key)));
    return h;
}

template<>
bool core::eq(const i32& a, const i32& b) {
    return a == b;
}

template<>
addr_size core::hash(const core::str_view& key) {
    addr_size h = addr_size(core::simple_hash_32(key.data(), key.len));
    return h;
}

template<>
bool core::eq(const core::str_view& a, const core::str_view& b) {
    return a == b;
}

template <typename TVal, typename TAllocator>
using i32_hash_map = core::hash_map<i32, TVal, TAllocator>;
template <typename TVal, typename TAllocator>
using sv_hash_map = core::hash_map<core::str_view, TVal, TAllocator>;

template <typename K, typename V>
struct __test_kv { const K& first; const V& second; };

template <typename M, typename K, typename V>
void __test_verifyKeyVal(const M& m, const __test_kv<K, V>& kv) {
    auto& key = kv.first;
    auto& val = kv.second;

    auto a = m.get(key);
    Assert(a != nullptr, "Failed to get data for key");
    Assert(*a == val, "Value at key is incorrect");

    i32 found = 0;
    m.keys([&](const auto& k) {
        if (core::eq(k, key)) { found++; }
    });
    Assert(found == 1, "Key should be found exactly once");

    found = 0;
    m.values([&](const auto& v) {
        if (v == val) { found++; }
    });
    Assert(found > 0, "Value should be found at least once");
}

// test verify many key values with variadic
template <typename M, typename... KV>
void __test_verifyKeyVals(const M& m, const KV&... kv) {
    (__test_verifyKeyVal(m, kv), ...);
}

template <typename M>
void __test_verify_no_keys(const M& m) {
    bool noKeys = true;
    m.keys([&](const auto&) { noKeys = false; });
    Assert(noKeys, "Hash map should not have keys");
}

template <typename M>
void __test_verify_no_values(const M& m) {
    bool noValues = true;
    m.values([&](const auto&) { noValues = false; });
    Assert(noValues, "Hash map should not have values");
}

template <typename TAllocator>
i32 initialize_hash_map() {
    {
        i32_hash_map<i32, TAllocator> m;
        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }
    {
        sv_hash_map<i32, TAllocator> m;
        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }
    {
        i32_hash_map<i32, TAllocator> m(7);
        Assert(m.len() == 0);
        Assert(m.cap() > 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }
    {
        sv_hash_map<i32, TAllocator> m(7);
        Assert(m.len() == 0);
        Assert(m.cap() > 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }

    return 0;
}

template <typename TAllocator>
i32 put_move_copy_hash_map() {
    using core::sv;

    {
        i32_hash_map<i32, TAllocator> m;
        m.put(1, 1);
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1});
        Assert(m.len() == 1);
        Assert(!m.empty());
        m.put(2, 1);
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 1});
        Assert(m.len() == 2);
        Assert(!m.empty());
        m.put(2, 9);
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9});
        Assert(m.len() == 2);
        Assert(!m.empty());

        // Move m to m2

        i32_hash_map<i32, TAllocator> m2(core::move(m));
        Assert(m2.len() == 2);
        __test_verifyKeyVals(m2, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9});
        Assert(!m2.empty());

        // Verify m is empty

        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);

        // Copy back to m

        m = m2.copy();
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9});
        Assert(!m.empty());

        Assert(m2.len() == 2);
        __test_verifyKeyVals(m2, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9});
        Assert(!m2.empty());
    }

    {
        i32_hash_map<i32, TAllocator> m(2);
        m.put(1, 1);
        Assert(m.len() == 1);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1});
        m.put(2, 1);
        Assert(m.len() == 2);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 1});
        m.put(2, 9);
        Assert(m.len() == 2);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9});
        m.put(3, 5);
        Assert(m.len() == 3);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{2, 9}, __test_kv<i32, i32>{3, 5});
        m.put(4, 5);
        Assert(m.len() == 4);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1},
                                __test_kv<i32, i32>{2, 9},
                                __test_kv<i32, i32>{3, 5},
                                __test_kv<i32, i32>{4, 5});

        // Move m to m2

        auto m2 = core::move(m);
        Assert(m2.len() == 4);
        Assert(!m2.empty());
        __test_verifyKeyVals(m2, __test_kv<i32, i32>{1, 1},
                                 __test_kv<i32, i32>{2, 9},
                                 __test_kv<i32, i32>{3, 5},
                                 __test_kv<i32, i32>{4, 5});

        // Verify m is empty

        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);

        // Copy back to m

        m = m2.copy();

        Assert(m.len() == 4);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1},
                                 __test_kv<i32, i32>{2, 9},
                                 __test_kv<i32, i32>{3, 5},
                                 __test_kv<i32, i32>{4, 5});
    }

    {
        sv_hash_map<i32, TAllocator> m;
        m.put(sv("1"), 1);
        Assert(m.len() == 1);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1});
        m.put(sv("2"), 1);
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 1});
        m.put(sv("2"), 9);
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 9});

        // Move m to m2

        auto m2 = core::move(m);
        Assert(m2.len() == 2);
        __test_verifyKeyVals(m2, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 9});

        // Verify m is empty

        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);

        // Copy back to m

        m = m2.copy();
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 9});

        Assert(m2.len() == 2);
        __test_verifyKeyVals(m2, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 9});
    }

    {
        sv_hash_map<i32, TAllocator> m(2);
        m.put(sv("1"), 1);
        Assert(m.len() == 1);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1});
        m.put(sv("2"), 1);
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 1});
        m.put(sv("2"), 9);
        Assert(m.len() == 2);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1}, __test_kv<core::str_view, i32>{sv("2"), 9});
        m.put(sv("3"), 5);
        Assert(m.len() == 3);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1},
                                __test_kv<core::str_view, i32>{sv("2"), 9},
                                __test_kv<core::str_view, i32>{sv("3"), 5});
        m.put(sv("4"), 5);
        Assert(m.len() == 4);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1},
                                __test_kv<core::str_view, i32>{sv("2"), 9},
                                __test_kv<core::str_view, i32>{sv("3"), 5},
                                __test_kv<core::str_view, i32>{sv("4"), 5});

        // Move m to m2

        auto m2 = core::move(m);
        Assert(m2.len() == 4);
        __test_verifyKeyVals(m2, __test_kv<core::str_view, i32>{sv("1"), 1},
                                 __test_kv<core::str_view, i32>{sv("2"), 9},
                                 __test_kv<core::str_view, i32>{sv("3"), 5},
                                 __test_kv<core::str_view, i32>{sv("4"), 5});

        // Verify m is empty

        Assert(m.len() == 0);
        Assert(m.cap() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);

        // Copy back to m

        m = m2.copy();
        Assert(m.len() == 4);
        __test_verifyKeyVals(m, __test_kv<core::str_view, i32>{sv("1"), 1},
                                 __test_kv<core::str_view, i32>{sv("2"), 9},
                                 __test_kv<core::str_view, i32>{sv("3"), 5},
                                 __test_kv<core::str_view, i32>{sv("4"), 5});


        Assert(m2.len() == 4);
        __test_verifyKeyVals(m2, __test_kv<core::str_view, i32>{sv("1"), 1},
                                 __test_kv<core::str_view, i32>{sv("2"), 9},
                                 __test_kv<core::str_view, i32>{sv("3"), 5},
                                 __test_kv<core::str_view, i32>{sv("4"), 5});
    }

    return 0;
}

template <typename TAllocator>
i32 remove_from_hash_map() {
    {
        i32_hash_map<i32, TAllocator> m(2);
        m.put(1, 1);
        m.remove(1);

        Assert(m.len() == 0);
        Assert(m.cap() > 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }

    {
        i32_hash_map<i32, TAllocator> m;
        m.put(1, 1);
        m.put(2, 1);
        m.put(3, 1);

        m.remove(2);
        Assert(m.len() == 2);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 1}, __test_kv<i32, i32>{3, 1});

        m.remove(1);
        Assert(m.len() == 1);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{3, 1});

        m.remove(3);
        Assert(m.len() == 0);
        Assert(m.empty());
        __test_verify_no_keys(m);
        __test_verify_no_values(m);
    }

    {
        i32_hash_map<i32, TAllocator> m(2);

        m.put(1, 1);
        m.put(2, 1);

        m.remove(1);
        m.remove(2);

        m.put(1, 7);
        m.put(2, 8);
        Assert(m.len() == 2);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 7}, __test_kv<i32, i32>{2, 8});

        m.put(3, 9);
        Assert(m.len() == 3);
        Assert(!m.empty());
        __test_verifyKeyVals(m, __test_kv<i32, i32>{1, 7}, __test_kv<i32, i32>{2, 8}, __test_kv<i32, i32>{3, 9});
    }

    return 0;
}

template <typename TAllocator>
i32 complex_types_in_hash_map() {
    using core::sv;

    {
        defer { SVCT::nextId = 0; };

        sv_hash_map<SVCT, TAllocator> m(2); // should not call the ctor!

        Assert(SVCT::nextId == 0);

        m.put(sv("1"), SVCT{});
        m.put(sv("2"), SVCT{});
        m.put(sv("3"), SVCT{});
        m.put(sv("4"), SVCT{});

        Assert(SVCT::nextId == 4);
        Assert(m.get(sv("1")));
        Assert(m.get(sv("2")));
        Assert(m.get(sv("3")));
        Assert(m.get(sv("4")));

        Assert(m.get(sv("1"))->a == 0);
        Assert(m.get(sv("2"))->a == 1);
        Assert(m.get(sv("3"))->a == 2);
        Assert(m.get(sv("4"))->a == 3);
    }

    {
        defer { CT::resetAll(); };

        sv_hash_map<CT, TAllocator> m(2);

        Assert(CT::dtorsCalled() == 0);
        Assert(CT::totalCtorsCalled() == 0);
        Assert(CT::assignmentsTotalCalled() == 0);

        m.put(sv("1"), CT{});

        Assert(CT::defaultCtorCalled() == 1);
        Assert(CT::copyCtorCalled() == 0);
        Assert(CT::moveCtorCalled() == 0);
        Assert(CT::assignmentsCopyCalled() == 0);
        Assert(CT::assignmentsMoveCalled() == 1); // put calls move assignment
        Assert(CT::dtorsCalled() == 1);
        Assert(CT::totalCtorsCalled() == 1);
        Assert(CT::assignmentsTotalCalled() == 1);

        CT c;
        m.put(sv("2"), c);

        Assert(CT::defaultCtorCalled() == 2);
        Assert(CT::copyCtorCalled() == 0);
        Assert(CT::moveCtorCalled() == 0);
        Assert(CT::assignmentsCopyCalled() == 1); // put calls copy assignment
        Assert(CT::assignmentsMoveCalled() == 1);
        Assert(CT::dtorsCalled() == 1);
        Assert(CT::totalCtorsCalled() == 2);
        Assert(CT::assignmentsTotalCalled() == 2);

        // This triggers a resize
        m.put(sv("3"), core::move(c));

        Assert(CT::defaultCtorCalled() == 2);
        Assert(CT::copyCtorCalled() == 0);
        Assert(CT::moveCtorCalled() == 0);
        Assert(CT::assignmentsCopyCalled() == 1);
        Assert(CT::assignmentsMoveCalled() == 4); // old acount (1) += 2 assignments to resize + 1 assignment for put
        Assert(CT::dtorsCalled() == 3); // old dcount (1) += 1 before put is called + 1 for put
        Assert(CT::totalCtorsCalled() == 2);
        Assert(CT::assignmentsTotalCalled() == 5);

        m.put(sv("3"), CT{}); // update

        Assert(CT::defaultCtorCalled() == 3);
        Assert(CT::copyCtorCalled() == 0);
        Assert(CT::moveCtorCalled() == 0);
        Assert(CT::assignmentsCopyCalled() == 1);
        Assert(CT::assignmentsMoveCalled() == 5);
        Assert(CT::dtorsCalled() == 4);
        Assert(CT::totalCtorsCalled() == 3);
        Assert(CT::assignmentsTotalCalled() == 6);

        // Make sure data is appropriately copied and moved in the containder.

        auto v1 = m.get(sv("1")); Assert(v1);
        auto v2 = m.get(sv("2")); Assert(v2);
        auto v3 = m.get(sv("3")); Assert(v3);

        Assert(v1->a == CT::defaultValue);
        Assert(v2->a == CT::defaultValue);
        Assert(v3->a == CT::defaultValue);

        v1->a = 1;
        Assert(v1->a == 1);
        Assert(v2->a == CT::defaultValue);
        Assert(v3->a == CT::defaultValue);

        v2->a = 2;
        Assert(v1->a == 1);
        Assert(v2->a == 2);
        Assert(v3->a == CT::defaultValue);

        v3->a = 3;
        Assert(v1->a == 1);
        Assert(v2->a == 2);
        Assert(v3->a == 3);
    }

    {
        // Make sure that put calls the destructor and memory leaks are not possible.

        struct tmp {
            char* data = nullptr;
            tmp() { data = reinterpret_cast<char*>(TAllocator::alloc(1)); }
            tmp(const tmp& other) {
                data = reinterpret_cast<char*>(TAllocator::alloc(1));
                core::memcopy(data, other.data, 1);
            }
            tmp(tmp&& other) {
                if (this == &other) return;
                data = other.data;
                other.data = nullptr;
            }
            ~tmp() {
                free();
            }

            void free() {
                if (data) {
                    TAllocator::free(data);
                }
                data = nullptr;
            }

            tmp& operator=(const tmp& other) {
                if (this == &other) return *this;
                free();
                data = reinterpret_cast<char*>(TAllocator::alloc(1));
                core::memcopy(data, other.data, 1);
                return *this;
            }
            tmp& operator=(tmp&& other) {
                if (this == &other) return *this;
                free();
                data = other.data;
                other.data = nullptr;
                return *this;
            }
        };

        sv_hash_map<tmp, TAllocator> m(2);
        tmp t;
        m.put(sv("1"), t);
        m.put(sv("1"), tmp{});

        // m.get(sv("1"))->data = nullptr; // this will be detected as a memory leak.
    }

    return 0;
}

