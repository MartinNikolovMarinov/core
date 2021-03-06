#pragma once

#include "API.h"
#include "types.h"

namespace core
{

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
CORE_API_EXPORT void RandInit();
CORE_API_EXPORT void RandInit(u64 seed);
CORE_API_EXPORT u32  RandU32();
CORE_API_EXPORT u32  RandU32(u32 min, u32 max);
CORE_API_EXPORT u64  RandU64();
CORE_API_EXPORT u64  RandU64(u64 min, u64 max);
CORE_API_EXPORT i32  RandI32();
CORE_API_EXPORT i32  RandI32(i32 min, i32 max);
CORE_API_EXPORT i64  RandI64();
CORE_API_EXPORT i64  RandI64(i64 min, i64 max);
CORE_API_EXPORT void RandCptr(char* out, ptr_size len);

} // namespace core
