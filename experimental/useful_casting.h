#pragma once

namespace core {

template<class T>
struct add_const { typedef const T type; };
template <class T>
constexpr typename add_const<T>::type& as_const(T& t) { return t; }

}
