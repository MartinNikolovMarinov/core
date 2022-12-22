#pragma once

namespace core {

template<typename T> struct remove_ref      { typedef T type; };
template<typename T> struct remove_ref<T&>  { typedef T type; };
template<typename T> struct remove_ref<T&&> { typedef T type; };

template<typename T>
constexpr typename remove_ref<T>::type && move(T && arg) {
    return static_cast<typename remove_ref<T>::type &&>(arg);
}

template<typename T> struct is_L_value     { static constexpr bool value = false; };
template<typename T> struct is_L_value<T&> { static constexpr bool value = true; };

template<typename T>
constexpr T&& forward(typename remove_ref<T>::type & arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_ref<T>::type && arg) {
    static_assert(!is_L_value<T>::value, "invalid rvalue to lvalue conversion");
    return static_cast<T&&>(arg);
}

}
