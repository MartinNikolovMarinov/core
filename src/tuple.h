#pragma once

#include "API.h"
#include "types.h"

namespace core
{

using namespace coretypes;

template <typename...> struct CORE_API_EXPORT Tuple;
template <typename T1, typename T2> struct Tuple<T1, T2> { T1 a; T2 b; };
template <typename T1, typename T2, typename T3> struct Tuple<T1, T2, T3> { T1 a; T2 b; T3 c; };
template <typename T1, typename T2, typename T3, typename T4> struct Tuple<T1, T2, T3, T4> { T1 a; T2 b; T3 c; T4 d; };

} // namespace core
