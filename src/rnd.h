#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

/**
 * Initializes the global state needed for pseudo-random number generation.
 * By default the initial seed is based on system clock time.
 *
 * The algorithm/s chosen will prioritize speed above security, therefore:
 * - These functions should not be used for applications that need a
 *   cryptographically-secure random number generation.
 * - The random number generation is not thread safe.
 * - None of the provided parameters, in any of the functions, will
 *   be checked for validity. If, for example, a min value is bigger
 *   that max value the behaviour is undefined.
*/
CORE_API_EXPORT void rnd_init();
CORE_API_EXPORT void rnd_init(u64 seed);
CORE_API_EXPORT u32  rnd_u32();
CORE_API_EXPORT u32  rnd_u32(u32 min, u32 max);
CORE_API_EXPORT u64  rnd_u64();
CORE_API_EXPORT u64  rnd_u64(u64 min, u64 max);
CORE_API_EXPORT i32  rnd_i32();
CORE_API_EXPORT i32  rnd_i32(i32 min, i32 max);
CORE_API_EXPORT i64  rnd_i64();
CORE_API_EXPORT i64  rnd_i64(i64 min, i64 max);
CORE_API_EXPORT f32  rnd_f32();
CORE_API_EXPORT f32  rnd_f32(f32 min, f32 max);
CORE_API_EXPORT f64  rnd_f64();
CORE_API_EXPORT f64  rnd_f64(f64 min, f64 max);
CORE_API_EXPORT void rnd_cptr(char* out, ptr_size len);

} // namespace core
