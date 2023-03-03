#pragma once

#include <type_traits>
#include <core_traits.h>

namespace core {

template<typename ...T>
using args_common_type = typename std::common_type<T...>::type;

template<typename T, typename... Ts>
using args_have_common_type = typename std::conjunction<std::is_same<Ts, T>...>::type;
template<typename T, typename... Ts>
inline constexpr bool args_have_common_type_v = args_have_common_type<T, Ts...>::value;

} // namespace core
