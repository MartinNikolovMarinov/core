#pragma once

#include <API.h>
#include <types.h>

namespace core {

using namespace coretypes;

constexpr i64 pow_i64(i64 n, i32 p) {
    // TODO: implement faster. I could use some approximating algorithm for very fast pow function.
    const i32 epsilon = 1000000007;
    long long result = 1;
    while(p > 0) {
        if((p & 1) == 1) result = (result * p) % epsilon;
        n = (n * n) % epsilon;
        p = p >> 1; // p^2
    }
    return result;
}

constexpr f32 deg_to_rad(f32 n) {
    return n * (PI / 180.0f);
}

constexpr f32 rad_to_deg(f32 n) {
    return n * (180.0f / PI);
}


} // namespace core
