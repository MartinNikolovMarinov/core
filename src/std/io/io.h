
#include <API.h>
#include <types.h>
#include <core_error.h>

namespace core::io
{

using namespace coretypes;

// The core::io interface:

/**
 * @brief Implementing this interface allows generic programming over io::Read.
 *        A buf argument equal to nullptr should be treated as a valid read and any underlining counters should be
 *        advanced appropriately, but the data should be discarded.
 *
 * @param[in] reader The reader to read from
 * @param[out] buf The buffer to read into
 * @param[in] size The size of the buffer
 * @return A TRes which should implement error::IsErr and error::Err
*/
template<typename TR, typename TRes> CORE_API_EXPORT TRes Read(TR& reader, void* buf, u64 size) noexcept;

/**
 * @brief Implementing this interface allows generic programming over io::Close.
 *
 * @param[in] closer The closer
 * @return A TRes which should implement error::IsErr and error::Err
*/
template<typename TC, typename TRes> CORE_API_EXPORT TRes Close(TC& closer) noexcept;

/**
 * @brief This function works on the TRes from another io function. Could be the read N bytes, the written N bytes, or
 *        somthing else along those lines.
 *
 * @param[in] res The result from another io function
 * @return The number of bytes read/written
*/
template<typename TRes> CORE_API_EXPORT u64 N(const TRes& res) noexcept;

/**
 * @brief This function works on the TRes from another io function.
 *        The interface allows checking for io errors in a generic way.
 *
 * @param[in] res The result from another io function
 * @return True if the io function failed, false otherwise
*/
template<typename TRes> CORE_API_EXPORT bool  IsErr(const TRes& res) noexcept;

/**
 * @brief This function works on the TRes from another io function.
 *        The interface allows getting io errors in a generic way.
 *        Always check with IsErr before calling this function.
 *        It still should be guaranteed that there is no crash when calling this function when IsErr returns false.
 *
 * @param[in] res The result from another io function
 * @return The error if the io function failed, undefined behavior otherwise
*/
template<typename TRes> CORE_API_EXPORT core::error::Error Err(const TRes& res) noexcept;

} // namspace core::io
