#include <core.h>
#include <iostream>

using namespace coretypes;

#define RunTest(test)                                                       \
    std::cout << "\t[TEST RUNNING] " << #test << '\n';                      \
    test();                                                                 \
    std::cout << "\t[TEST \x1b[32mPASSED\x1b[0m] " << #test << std::endl;

#define RunTestSuite(suite)                                                 \
    std::cout << "[SUITE RUNNING] " << #suite << std::endl;                 \
    suite();                                                                \
    std::cout << "[SUITE \x1b[32mPASSED\x1b[0m] " << #suite << std::endl;

// #include "test/run_tests.cpp"

// i32 main(i32, const char **) {
//     core::set_global_assert_handler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
//         std::cout << "[ASSERTION] [EXPR]: " << failedExpr
//                 << " [FILE]: " << file
//                 << " [LINE]: " << line
//                 << " [MSG]: " << errMsg
//                 << std::endl; // flush stream!
//         return true;
//     });

//     // Print compiler
//     if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
//     if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
//     if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
//     if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

//     // Print OS
//     if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
//     if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
//     if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
//     if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

//     run_all_tests();
//     // RunAllSTDTests();

//     std::cout << '\n';
//     std::cout << "\x1b[32m\x1b[1mTests OK\x1b[0m\n";
//     return 0;
// }

#include <cstring>

// NOTE: Implementation of std allocator:
namespace core {

struct std_allocator {
    void* alloc(ptr_size size) noexcept {
        return std::malloc(size);
    }

    template<typename T, typename ...Args>
    T* construct(T* out, Args... args) noexcept {
        void* p = alloc(sizeof(T));
        if (p != nullptr) out = new (p) T(core::forward<Args>(args)...);
        return out;
    }

    void free(void* ptr) noexcept {
        std::free(ptr);
    }

    void clear() noexcept {}

    ptr_size used_mem() const noexcept { return 0; }

    using on_oom_fp = void(*)(void* userData);
    void on_out_of_memory(on_oom_fp cb) noexcept;
};

}

// NOTE: User defined allocators:
namespace core {

using allocator_id = i32;

constexpr allocator_id default_allocator_id() { return 0; }

constexpr std_allocator g_default_allocator;

constexpr auto get_allocator(allocator_id id) {
    switch (id) {
        case 0: return g_default_allocator;
        default: return g_default_allocator;
    }
}

}

namespace core {

enum IteratorResponse { ReadBeforeBegin, ReadPastEnd };

} // namespace core


// FIXME: Check the asm that the arr code generates. It should be a very cheap abstraction if it's done right.

// NOTE: Implementation of arr:
namespace core {

using namespace coretypes;

template<typename T, core::allocator_id AllocId = core::default_allocator_id()>
struct arr {
    using data_type = T;
    using size_type = u64;

    constexpr arr() : m_data(nullptr), m_cap(0), m_len(0) {}
    constexpr arr(data_type *data, size_type len) : m_data(data), m_cap(len), m_len(len) {}
    constexpr arr(data_type *data, size_type cap, size_type len) : m_data(data), m_cap(cap), m_len(len) {
        Assert(m_cap >= m_len);
    }

    constexpr i32        allocatorId() const { return AllocId; }
    constexpr size_type  cap()         const { return m_cap; }
    constexpr size_type  len()         const { return m_len; }
    constexpr data_type* data()        const { return m_data; }
    constexpr bool       empty()       const { return m_len == 0; }
    constexpr void       clear()             { m_len = 0; }

    constexpr data_type& at(size_type idx)         { Assert(idx <= m_len); return m_data[idx]; }
    constexpr data_type& operator[](size_type idx) { return at(idx); }

    constexpr arr& append(const data_type& val) {
        if (m_len == m_cap) {
            resize(m_cap == 0 ? 1 : m_cap * 2);
        }
        m_data[m_len++] = val;
        return *this;
    }

    constexpr void resize(size_type newCap) {
        if (newCap == 0) {
            m_len = 0;
            m_cap = 0;
            core::free(core::get_allocator(AllocId), m_data);
            return;
        }
        if (newCap <= m_cap) {
            m_len = m_len > newCap ? newCap : m_len;
            m_cap = newCap;
            return;
        }

        data_type* newData = (data_type *)core::alloc(core::get_allocator(AllocId), newCap * sizeof(data_type));
        Assert(newData != nullptr); // TODO: handle OOM
        if (m_data != nullptr) {
            core::memcopy(newData, m_data, m_len * sizeof(data_type));
            core::free(core::get_allocator(AllocId), m_data);
        }
        m_data = newData;
        m_cap = newCap;
    }

    struct iter {
        size_type idx;
        arr* array;
        core::expected<data_type*, core::IteratorResponse> next() {
            if (idx >= array->len()) return core::unexpected(core::IteratorResponse::ReadPastEnd);
            return &array->m_data[idx++];
        }
        core::expected<data_type*, core::IteratorResponse> prev() {
            if (idx <= 0) return core::unexpected(core::IteratorResponse::ReadBeforeBegin);
            return &array->m_data[idx--];
        }
    };

    constexpr iter begin() { return iter{0, this}; }
    constexpr iter end()   { return iter{m_len, this}; }

private:
    data_type *m_data;
    size_type m_cap;
    size_type m_len;
};

}

i32 main(i32, const char **) {
    core::arr<i32> arr;
    arr.append(1);
    arr.append(2);
    arr.append(3);
    arr.append(4);

    auto iter = arr.begin();
    while (true) {
        auto next = iter.next();
        if (next.has_value()) {
            i32* v = next.value();
            std::cout << *v << std::endl;
        } else {
            break;
        }
    }

    return 0;
}
