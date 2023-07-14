#pragma once

#include <system_checks.h>
#include <types.h>

namespace core {

using namespace coretypes;

template<typename T>
constexpr bool AlwaysFalse = false;

#pragma region True/False Type ----------------------------------------------------------------------------------------

struct TrueType  { static constexpr bool value = true; };
struct FalseType { static constexpr bool value = false; };
constexpr bool TrueType_v  = TrueType::value;
constexpr bool FalseType_v = FalseType::value;

#pragma endregion

#pragma region Remove Reference ---------------------------------------------------------------------------------------

template<typename T> struct RemoveRef      { typedef T type; };
template<typename T> struct RemoveRef<T&>  { typedef T type; };
template<typename T> struct RemoveRef<T&&> { typedef T type; };

#pragma endregion

#pragma region Is RValue/LValue ---------------------------------------------------------------------------------------

template<typename T> struct IsLValue     : FalseType {};
template<typename T> struct IsLValue<T&> : TrueType {};
template<typename T> constexpr bool IsLValue_v     = IsLValue<T>::value;
template<typename T> constexpr bool IsLValue_v<T&> = IsLValue<T&>::value;

template<typename T> struct IsRValue      : FalseType {};
template<typename T> struct IsRValue<T&&> : TrueType {};
template<typename T> constexpr bool IsRValue_v = IsRValue<T>::value;
template<typename T> constexpr bool IsRValue_v<T&&> = IsRValue<T&&>::value;

#pragma endregion

#pragma region Add/Remove LValue/RValue reference ---------------------------------------------------------------------

namespace detail {

template <typename T> struct TypeIdentity { using type = T; };

template <typename T> auto TryAddLValue(i32) -> TypeIdentity<T&>;
template <typename T> auto TryAddLValue(...) -> TypeIdentity<T>;

template <typename T> auto TryAddRValue(i32) -> TypeIdentity<T&&>;
template <typename T> auto TryAddRValue(...) -> TypeIdentity<T>;

} // namespace detail

template <typename T> struct AddLValue : decltype(detail::TryAddLValue<T>(0)) {};
template <typename T> struct AddRValue : decltype(detail::TryAddRValue<T>(0)) {};

template<typename T>
typename AddRValue<T>::type declval() noexcept {
    static_assert(AlwaysFalse<T>, "declval not allowed in an evaluated context");
}

#pragma endregion

#pragma region Compiletime Execution -----------------------------------------------------------------------------------

constexpr inline bool is_const_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
}

template <auto V>
constexpr auto force_consteval = V;

#pragma endregion

#pragma region Compiler Intrinsic Traits ------------------------------------------------------------------------------

template <typename T>
struct IsTriviallyDestructible {
#if (COMPILER_GCC == 1) || (COMPILER_CLANG == 1)
    static constexpr bool value = __has_trivial_destructor(T);
#elif (COMPILER_MSVC == 1)
    static constexpr bool value = __is_trivially_destructible(T);
#endif
};

template <typename T>
struct IsTriviallyCopyable {
#if (COMPILER_GCC == 1) || (COMPILER_CLANG == 1)
    static constexpr bool value = __has_trivial_copy(T);
#elif (COMPILER_MSVC == 1)
    static constexpr bool value = __is_trivially_copyable(T);
#endif
};

template <typename T>
struct IsTriviallyAssignable {
#if (COMPILER_GCC == 1) || (COMPILER_CLANG == 1)
    static constexpr bool value = __has_trivial_assign(T);
#elif (COMPILER_MSVC == 1)
    static constexpr bool value = __is_trivially_assignable(T);
#endif
};

/**
 * NOTE: Standard layout types as defined by the Microsoft documentation:
 *
 * When a class or struct does not contain certain C++ language features such as virtual functions
 * which are not found in the C language, and all members have the same access control, it is a standard-layout type. It
 * is memcopy-able and the layout is sufficiently defined that it can be consumed by C programs. Standard-layout types
 * can have user-defined special member functions. In addition, standard layout types have these characteristics:
 * - No virtual functions or virtual base classes.
 * - All non-static data members have the same access control.
 * - All non-static members of class type are standard-layout.
 * - Any base classes are standard-layout.
 * - Has no base classes of the same type as the first non-static data member.
 * - Meets one of these conditions:
 *  - No non-static data member in the most-derived class and no more than one base class with non-static data members, or
 *  - Has no base classes with non-static data members.
 */
template <typename T>
struct IsStandardLayout {
    static constexpr bool value = __is_standard_layout(T);
};

/**
 * NOTE: Trivial types as defined by the Microsoft documentation:
 *
 * When a class or struct in C++ has compiler-provided or explicitly defaulted special member functions,
 * then it is a trivial type. It occupies a contiguous memory area. It can have members with different access
 * specifiers. In C++, the compiler is free to choose how to order members in this situation. Therefore, you can memcopy
 * such objects but you cannot reliably consume them from a C program. A trivial type T can be copied into an array of
 * char or unsigned char, and safely copied back into a T variable. Note that because of alignment requirements, there
 * might be padding bytes between type members.
 *
 * Trivial types have a trivial default constructor, trivial copy constructor, trivial copy assignment operator and
 * trivial destructor. In each case, trivial means the constructor/operator/destructor is not user-provided and belongs
 * to a class that has:
 * - no virtual functions or virtual base classes,
 * - no base classes with a corresponding non-trivial constructor/operator/destructor
 * - no data members of class type with a corresponding non-trivial constructor/operator/destructor
 */
template <typename T>
struct IsTrivial {
    static constexpr bool value = __is_trivial(T);
};

// NOTE: Pods are both trivial and standard layout types.
template <typename T>
struct IsPOD {
    // __is_pod(T) is deprecated in c++20
    static constexpr bool value = __is_standard_layout(T) && __is_trivial(T);
};

template <typename T>
inline constexpr bool IsTriviallyDestructible_v = IsTriviallyDestructible<T>::value;
template <typename T>
inline constexpr bool IsTriviallyCopyable_v = IsTriviallyCopyable<T>::value;
template <typename T>
inline constexpr bool IsTriviallyAssignable_v = IsTriviallyAssignable<T>::value;
template <typename T>
inline constexpr bool IsStandardLayout_v = IsStandardLayout<T>::value;
template <typename T>
inline constexpr bool IsTrivial_v = IsTrivial<T>::value;
template <typename T>
inline constexpr bool IsPOD_v = IsPOD<T>::value;

#pragma endregion

} // namespace core
