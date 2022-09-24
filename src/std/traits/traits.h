#pragma once

namespace core::traits
{

template<typename ...T>
using template_args_common_type = typename std::common_type<T...>::type;

template<typename T, typename... Ts>
using template_args_are_of_type = typename std::conjunction<std::is_same<Ts, T>...>::type;
template<typename T, typename... Ts>
inline constexpr bool template_args_are_of_type_v = template_args_are_of_type<T, Ts...>::value;

}
