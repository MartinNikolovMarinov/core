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

#pragma region Compiler Intrinsic Traits ------------------------------------------------------------------------------

template<typename T>
struct IsTriviallyDestructible {
#if (COMPILER_GCC == 1) || (COMPILER_CLANG == 1)
    static constexpr bool value = __has_trivial_destructor(T);
#elif (COMPILER_MSVC == 1)
    static constexpr bool value = __is_trivially_destructible(T);
#endif
};

template<typename T>
inline constexpr bool IsTriviallyDestructible_v = IsTriviallyDestructible<T>::value;

#pragma endregion

} // namespace core
