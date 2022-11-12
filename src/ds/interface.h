#pragma once

#include <API.h>
#include <types.h>

namespace core::ds
{

using namespace coretypes;

template <typename T>
using VisitFn = bool(*)(T&);
template <typename T, typename UData>
using VisitFnUData = bool(*)(T&, UData& data);

template <typename T>
using EqualsFn = bool(*)(const T& a, const T& b);
template <typename T, typename UData>
using EqualsFnUData = bool(*)(const T& a, const T& b, UData& data);

template <typename T>
using CmpFn = i32(*)(const T& a, const T& b);
template <typename T, typename UData>
using CmpFnUData = i32(*)(const T& a, const T& b, UData& data);

template<typename T>
CORE_API_EXPORT bool Eq(T a, T b) {
    return a.Eq(b);
}

template<typename T>
CORE_API_EXPORT i32 Cmp(T a, T b) {
    return a.Cmp(b);
}

template <typename DS, typename T>
CORE_API_EXPORT void Append(DS& dataStruct, T& elem) {
    dataStruct.Append(elem);
}

template <typename DS>
CORE_API_EXPORT auto At(DS& dataStruct, i32 index) -> decltype(dataStruct.At(index)) {
    return dataStruct.At(index);
}

template <typename DS, typename T>
CORE_API_EXPORT void Put(DS& dataStruct, i32 index, T& elem) {
    dataStruct.Put(index, elem);
}

template <typename DS, typename T>
CORE_API_EXPORT T& Last(DS& dataStruct) {
    return dataStruct.Last();
}

template <typename DS, typename T>
CORE_API_EXPORT T& First(DS& dataStruct) {
    return dataStruct.First();
}

template <typename DS>
CORE_API_EXPORT ptr_size Len(DS& dataStruct) {
    return dataStruct.Len();
}

template <typename DS, typename T>
CORE_API_EXPORT void ForEach(DS& dataStruct, VisitFn<T> fn) {
    dataStruct.ForEach(fn);
}

template <typename DS, typename T, typename UData>
CORE_API_EXPORT void ForEach(DS& dataStruct, VisitFnUData<T, UData> fn, UData& data) {
    dataStruct.ForEach(fn, data);
}

template <typename DS, typename T>
CORE_API_EXPORT ptr_size FindIdx(DS& dataStruct, T& elem, EqualsFn<T> eq) {
    return dataStruct.Find(elem, eq);
}

template <typename DS, typename T, typename UData>
CORE_API_EXPORT ptr_size FindIdx(DS& dataStruct, T& elem, EqualsFnUData<T, UData> eq, UData& userData) {
    return dataStruct.Find(elem, eq, userData);
}

template <typename DS, typename T>
CORE_API_EXPORT T& Find(DS& dataStruct, T& elem, EqualsFn<T> eq) {
    return dataStruct.Find(elem, eq);
}

template <typename DS, typename T, typename UData>
CORE_API_EXPORT T& Find(DS& dataStruct, T& elem, EqualsFnUData<T, UData> eq, UData& userData) {
    return dataStruct.Find(elem, eq, userData);
}

template <typename DS, typename T>
CORE_API_EXPORT bool Contains(DS& dataStruct, T& elem, EqualsFn<T> eq) {
    return dataStruct.Contains(elem, eq);
}

template <typename DS, typename T, typename UData>
CORE_API_EXPORT bool Contains(DS& dataStruct, T& elem, EqualsFnUData<T, UData> eq, UData& userData) {
    return dataStruct.Contains(elem, eq, userData);
}

template <typename DS>
CORE_API_EXPORT void Clear(DS& dataStruct) {
    dataStruct.Clear();
}

} // namespace core
