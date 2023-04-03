#pragma once

#include <string>
#include <std/mat.h>

namespace core {

template <i32 TRow, i32 TCol, typename T>
std::string to_string(const mat<TRow, TCol, T>& m) {
    std::string ret;
    ret += "{";
    for (i32 i = 0; i < m.dimensionsRows(); ++i) {
        ret += "{";
        for (i32 j = 0; j < m.dimensionsCols(); ++j) {
            ret += std::to_string(m[i][j]);
            if (j != m.dimensionsCols() - 1) {
                ret += ", ";
            }
        }
        ret += "}";
        if (i != m.dimensionsRows() - 1) {
            ret += ", ";
        }
    }
    ret += "}";
    return ret;
}

} // namespace core

