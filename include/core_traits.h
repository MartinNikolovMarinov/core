#pragma once

#include <core_system_checks.h>
#include <core_types.h>

#include <type_traits>

namespace core {

using namespace coretypes;

template<typename> constexpr bool always_false = false;

template <typename> struct _is_char  { static constexpr bool value = std::false_type::value; };
template <> struct _is_char<char>    { static constexpr bool value = std::true_type::value; };
template <> struct _is_char<uchar>   { static constexpr bool value = std::true_type::value; };
template <> struct _is_char<schar>   { static constexpr bool value = std::true_type::value; };
template <typename T> struct is_char { static constexpr bool value = _is_char<std::remove_cv_t<T>>::value; };

template <typename T> constexpr bool is_char_v = is_char<T>::value;

template <typename> struct _is_float  { static constexpr bool value = std::false_type::value; };
template <> struct _is_float<f32>     { static constexpr bool value = std::true_type::value; };
template <> struct _is_float<f64>     { static constexpr bool value = std::true_type::value; };
template <typename T> struct is_float { static constexpr bool value = _is_float<std::remove_cv_t<T>>::value; };

template <typename T> constexpr bool is_float_v = is_float<T>::value;

template <typename> struct _is_signed  { static constexpr bool value = std::false_type::value; };
template <> struct _is_signed<i8>      { static constexpr bool value = std::true_type::value; };
template <> struct _is_signed<i16>     { static constexpr bool value = std::true_type::value; };
template <> struct _is_signed<i32>     { static constexpr bool value = std::true_type::value; };
template <> struct _is_signed<i64>     { static constexpr bool value = std::true_type::value; };
template <typename T> struct is_signed { static constexpr bool value = _is_signed<std::remove_cv_t<T>>::value; };

template <typename T> constexpr bool is_signed_v = is_signed<T>::value;

template <typename> struct _is_unsigned  { static constexpr bool value = std::false_type::value; };
template <> struct _is_unsigned<u8>      { static constexpr bool value = std::true_type::value; };
template <> struct _is_unsigned<u16>     { static constexpr bool value = std::true_type::value; };
template <> struct _is_unsigned<u32>     { static constexpr bool value = std::true_type::value; };
template <> struct _is_unsigned<u64>     { static constexpr bool value = std::true_type::value; };
template <typename T> struct is_unsigned { static constexpr bool value = _is_unsigned<std::remove_cv_t<T>>::value; };

template <typename T> constexpr bool is_unsigned_v = is_unsigned<T>::value;

template <typename T> struct is_integral { static constexpr bool value = is_unsigned_v<T> || is_signed_v<T>; };

template <typename T> constexpr bool is_integral_v = is_integral<T>::value;

template <typename T> struct is_arithmetic { static constexpr bool value = is_integral_v<T> || is_float_v<T>; };

template <typename T> constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

namespace detail {

template <class T, class U>
concept SameHelper = std::is_same_v<T, U>;

} // namespace detail

template <typename T, typename U>
concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

namespace detail {
constexpr bool is_const_evaluated() noexcept { return __builtin_is_constant_evaluated(); }
} // namespace detail

// Using a macro here to avoid mistakes where I put is_const_evaluated() in a constexpr if statement.
#define IS_CONST_EVALUATED if (core::detail::is_const_evaluated())
#define IS_NOT_CONST_EVALUATED if (!core::detail::is_const_evaluated())

template <auto V>
constexpr auto force_consteval = V;

} // namespace core
