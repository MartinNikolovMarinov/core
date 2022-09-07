
#include <API.h>
#include <types.h>
#include <core_error.h>

namespace core::io
{

using namespace coretypes;

// TODO: Comment these interfaces properly

// Read Result Interface
template<typename TRes> CORE_API_EXPORT bool         IsErr(const TRes& res);
template<typename TRes> CORE_API_EXPORT error::Error Err(const TRes& res);
template<typename TRes> CORE_API_EXPORT u64          N(const TRes& res);

// Reader Interface
//
// A buf argument equal to nullptr should be treated as a valid read and any underlining counters should be
// advanced appropriately, but the data should be discarded.
template<typename TR, typename TRes> CORE_API_EXPORT TRes Read(TR& reader, void* buf, u64 size);

// Close Interface:
template<typename TC, typename TRes> CORE_API_EXPORT TRes Close(TC& closer);

} // namspace core::io
