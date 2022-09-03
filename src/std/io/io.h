
#include <API.h>
#include <types.h>
#include <core_error.h>

namespace core::io
{

using namespace coretypes;

// Read Result Interface
template<typename TRes> CORE_API_EXPORT static bool         IsErr(const TRes& res);
template<typename TRes> CORE_API_EXPORT static error::Error Err(const TRes& res);
template<typename TRes> CORE_API_EXPORT static u64          N(const TRes& res);

// Reader Interface:
template<typename TR, typename TRes> CORE_API_EXPORT static TRes Read(TR& reader, void* buf, u64 size);

// Close Interface:
template<typename TC, typename TRes> CORE_API_EXPORT static TRes Close(TC& closer);

} // namspace core::io
