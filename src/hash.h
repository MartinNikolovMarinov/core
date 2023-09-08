#pragma once

#include "API.h"
#include "types.h"

namespace core {

using namespace coretypes;

/**
 * \brief A very simple and fast hashing function to provide as a default hasher. Will have collisions and should not be
 *        used in serious applications that require unique hashes. It can however be used as a decent hash by appending
 *        some additional state.
 *
 * \param input The input data to hash.
 * \param len The length of the input data.
 *
 * \return The hash.
*/
CORE_API_EXPORT u32 simple_hash(const void* input, addr_size len);

} // namespace core
