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

// // #include "test/run_tests.cpp"
// // #include "test/std/run_std_tests.cpp"

// // i32 main() {
// //     core::SetGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* errMsg) {
// //         std::cout << "[ASSERTION] [EXPR]: " << failedExpr
// //                 << " [FILE]: " << file
// //                 << " [LINE]: " << line
// //                 << " [MSG]: " << errMsg
// //                 << std::endl; // flush stream!
// //         return true;
// //     });

// //     if (COMPILER_CLANG == 1)   { std::cout << "[COMPILER] COMPILER_CLANG" << std::endl; }
// //     if (COMPILER_GCC == 1)     { std::cout << "[COMPILER] COMPILER_GCC" << std::endl; }
// //     if (COMPILER_MSVC == 1)    { std::cout << "[COMPILER] COMPILER_MSVC" << std::endl; }
// //     if (COMPILER_UNKNOWN == 1) { std::cout << "[COMPILER] COMPILER_UNKNOWN" << std::endl; }

// //     if (OS_WIN == 1)     { std::cout << "[OS] OS_WIN" << std::endl; }
// //     if (OS_LINUX == 1)   { std::cout << "[OS] OS_LINUX" << std::endl; }
// //     if (OS_UNIX == 1)    { std::cout << "[OS] OS_UNIX" << std::endl; }
// //     if (OS_UNKNOWN == 1) { std::cout << "[OS] OS_UNKNOWN" << std::endl; }

// //     RunAllTests();
// //     RunAllSTDTests();

// //     std::cout << '\n';
// //     std::cout << "\x1b[32m\x1b[1mTests OK\x1b[0m\n";
// //     return 0;
// // }

// #include <alloc/interface.h>
// #include <std/alloc/std.h>

// // FIXME: How do I handle allocator switching?
// //        Maybe just register all allocators globally and use tags to switch between them in templates?
// //        BUT I might need to be able to switch between allocators at runtime, which won't be possible.
// //        Or maybe I don't actually care about that ?

// // template <typename TAlloc>
// // TAlloc GetDefaultAllocator() {
// //     static core::alloc::StdAllocator stdalloc;
// //     return stdalloc;
// // }

// using DefaultAllocator = core::alloc::StdAllocator;
// constexpr u32 ALLOC_REGISTRY_SIZE = 2;
// void *gAllocRegistry[ALLOC_REGISTRY_SIZE];

// template <i32 TAllocId>
// auto* GetAllocator() {
//     if constexpr (TAllocId == 0) {
//         return reinterpret_cast<DefaultAllocator*>(gAllocRegistry[0]);
//     }
//     else if constexpr (TAllocId == 1) {
//         return reinterpret_cast<core::alloc::StaticBumpAllocator<1048>*>(gAllocRegistry[1]);
//     }
//     else {
//         static_assert(TAllocId < ALLOC_REGISTRY_SIZE, "Invalid allocator id");
//         return DefaultAllocator();
//     }
// }

// void SetAllocator(i32 allocId, void *alloc) {
//     gAllocRegistry[allocId] = alloc;
// }

// template<typename T, i32 TAllocId = 0>
// struct Arr {

//     Arr() : data(nullptr), len(0), cap(0) {};

//     void Testing() {
//         if (data == nullptr) {
//             data = reinterpret_cast<T*>(core::alloc::Alloc(*GetAllocator<TAllocId>(), sizeof(T) * 10));
//         }
//     }

// private:
//     T* data;
//     ptr_size len;
//     ptr_size cap;
// };

// int main() {
//     SetAllocator(0, new DefaultAllocator());
//     SetAllocator(1, new core::alloc::StaticBumpAllocator<1048>());

//     Arr<i32> darr = Arr<i32>();
//     darr.Testing();

//     Arr<i32, 1> sarr = Arr<i32, 1>();
//     sarr.Testing();

//     constexpr i32 test = sizeof(Arr<i32>);
//     std::cout << test << std::endl;
//     constexpr i32 test2 = sizeof(std::vector<i32>);
//     std::cout << test2 << std::endl;
//     return 0;
// }

#include <variant>

struct Impl1 {
    int A(int a) { return a * 10; }
};

struct Impl2 {
    int A(int a) { return a * 5; }
};

// using InterfaceImplVariant = std::variant<Impl1, Impl2>;
// struct InterfaceImplVariant {
//     constexpr InterfaceImplVariant() : a({}), type(0) {}
//     constexpr InterfaceImplVariant(Impl1 a) : a(a), type(1) {}
//     constexpr InterfaceImplVariant(Impl2 b) : b(b), type(2) {}
//     constexpr ~InterfaceImplVariant() {
//         if      (type == 1) a.~Impl1();
//         else if (type == 2) b.~Impl2();
//     }

//     constexpr InterfaceImplVariant& operator=(const InterfaceImplVariant& other) {
//         type = other.type;
//         if      (type == 1) a = other.a;
//         else if (type == 2) b = other.b;
//         return *this;
//     }

//     constexpr int index() const { return type - 1; }

//     template<int Idx>
//     constexpr auto get() {
//         if      constexpr (Idx == 1) return a;
//         else if constexpr (Idx == 2) return b;
//     }

//     union {
//         Impl1 a;
//         Impl2 b;
//     };
//     int type;
// };

#pragma region aligned_storage

template<u32 TLen, u32 TAlign>
struct aligned_storage {
    struct type {
        alignas(TAlign) u8 data[TLen];
    };
};

#pragma endregion

#pragma region static_max

template <unsigned int Arg1, unsigned int ...Rest>
struct static_max;

template <unsigned int Arg>
struct static_max<Arg> {
    static constexpr u32 value = Arg;
};

template <unsigned int arg1, unsigned int arg2, unsigned int ... others>
struct static_max<arg1, arg2, others...>
{
    static constexpr u32 value = arg1 >= arg2 ?
                                    static_max<arg1, others...>::value :
                                    static_max<arg2, others...>::value;
};

#pragma endregion

namespace
{

#pragma region variant helpers

template<u8 Idx, typename TCurr, typename... Ts>
struct variant_helper_rec {
    inline static void destroy(u8 idx, void* data) {
        if (idx == Idx) reinterpret_cast<TCurr*>(data)->~TCurr();
        else variant_helper_rec<Idx + 1, Ts...>::destroy(idx, data);
    }
    inline static void copy(u8 idx, void* src, void* dest) {
        if (idx == Idx) new (dest) TCurr(*reinterpret_cast<TCurr*>(src));
        else variant_helper_rec<Idx + 1, Ts...>::copy(idx, src, dest);
    }
    inline static void move(u8 idx, void* src, void* dest) {
        if (idx == Idx) new (dest) TCurr(std::move(*reinterpret_cast<TCurr*>(src)));
        else variant_helper_rec<Idx + 1, Ts...>::move(idx, src, dest);
    }
};

// Template recursion bottom:
template<u8 Idx, typename TCurr>
struct variant_helper_rec<Idx, TCurr> {
    inline static void destroy(u8, void*) {}
    inline static void copy(u8, void*, void*) {}
    inline static void move(u8, void*, void*) {}
};

template <typename... Ts>
struct variant_helper {
    inline static void destroy(u8 idx, void* data) {
        variant_helper_rec<0, Ts...>::destroy(idx, data);
    }
    inline static void copy(u8 idx, void* src, void* dest) {
        variant_helper_rec<0, Ts...>::copy(idx, src, dest);
    }
    inline static void move(u8 idx, void* src, void* dest) {
        variant_helper_rec<0, Ts...>::move(idx, src, dest);
    }
};

template<typename TCurr>
struct variant_helper_static;


// FIXME: why the void pointer nonsenes ?
template<typename TCurr>
struct variant_helper_static {
    inline static void move(void* from, void* to) {
        new (to) TCurr(std::move(*reinterpret_cast<TCurr*>(from)));
    }
    inline static void copy(const void* from, void* to) {
        new (to) TCurr(*reinterpret_cast<const TCurr*>(from));
    }
};

#pragma endregion

};

#pragma region Variant

// Given an u8 index, selects the i-th type from the list of item types.
template<u8 Idx, typename... Items> struct variant_alternative;

template<typename THead, typename... TTail>
struct variant_alternative<0, THead, TTail...> {
    using Type = THead;
};

template<u8 TIdx, typename THead, typename... TTail>
struct variant_alternative<TIdx, THead, TTail...> {
    using Type = typename variant_alternative<TIdx - 1, TTail...>::Type;
};

template<typename... Ts>
struct variant {
    static constexpr u32 DataSize = static_max<sizeof(Ts)...>::value;
    static constexpr u32 Alignment = static_max<alignof(Ts)...>::value;

    template<u8 TIdx>
    using alternative = typename variant_alternative<TIdx, Ts...>::Type;
    using helper = variant_helper<Ts...>;
    template<typename T>
    using helper_static = variant_helper_static<T>;

    variant() : tagId(-1) {}

    template<u8 TIdx>
    static variant create(alternative<TIdx>& value) {
        variant ret(TIdx);
        helper_static<alternative<TIdx>>::copy(&value, &ret.data);
        return ret;
    }
    template<u8 TIdx>
    static variant create(alternative<TIdx>&& value) {
        variant ret(TIdx);
        helper_static<alternative<TIdx>>::move(&value, &ret.data);
        return ret;
    }

    // Copy and move constructors
    variant(const variant<Ts...>& other) : tagId(other.tagId) {
        helper::copy(other.tagId, &other.data, &data);
    }
    variant(variant<Ts...>&& other) : tagId(other.tagId) {
        helper::move(other.tagId, &other.data, &data);
    }

    // Copy and move assignment operators
    variant<Ts...>& operator=(variant<Ts...>& rhs) {
        helper::destroy(tagId, &data);
        tagId = rhs.tagId;
        helper::copy(rhs.tagId, &rhs.data, &data);
        return *this;
    }
    variant<Ts...>& operator=(variant<Ts...>&& rhs) {
        helper::destroy(tagId, &data);
        tagId = rhs.tagId;
        helper::move(rhs.tagId, &rhs.data, &data);
        return *this;
    }

    template<u8 TIdx>
    alternative<TIdx>& get() {
        Assert(tagId == TIdx, "Invalid variant index");
        return *reinterpret_cast<alternative<TIdx>*>(&data);
    }

    template<u8 TIdx>
    void set(const alternative<TIdx>& value) {
        helper::destroy(tagId, data);
        tagId = TIdx;
        new (&data) alternative<TIdx>(value);
    }

    template<u8 TIdx>
    void set(alternative<TIdx>&& value) {
        helper::destroy(tagId, data);
        tagId = TIdx;
        new (&data) alternative<TIdx>(std::move(value));
    }

    ~variant() {
        helper::destroy(tagId, data);
    }

private:
    u8 tagId;
    u8 data[DataSize];

    variant (u32 tagId) : tagId(tagId) {}
};

#pragma endregion

// template <int N>
// constexpr int Test(InterfaceImplVariant arr[N]) {
//     int ret = 0;
//     for (int i = 0; i < N; i++) {
//         if (arr[i].index() == 0) {
//             Impl1 impl1 = arr[i].get<1>();
//             ret += impl1.A(i);
//         } else if(arr[i].index() == 1) {
//             Impl2 impl2 = arr[i].get<2>();
//             ret += impl2.A(i);
//         }
//     }
//     return ret;
// }

int main() {
    // Impl1 a;
    // Impl2 b;
    // InterfaceImplVariant arr[2];
    // arr[0] = a;
    // arr[1] = b;
    // return Test<2>(arr);
    variant<int, float> a;
    a = variant<int, float>::create<0>(5);
    return 0;
}
