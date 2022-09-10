#pragma once

#include <API.h>
#include <types.h>
#include <core_error.h>
#include <plt.h>
#include <io.h>

#include <vector>

namespace core::fs
{

using namespace coretypes;

struct CORE_API_EXPORT File {
    using Error = core::error::Error;
    using ErrorFile = core::error::ErrorValue<File, Error>;

    static constexpr u32 DEFAULT_BUF_SIZE = 1024;

    static constexpr std::string_view ERR_EOF = "EOF";

    File() = default;
    File(plt::FileDesc&& file) : m_file(std::move(file)) {}

    // Prevent copy
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;

    // Allow move
    File(File&& other) : m_file(std::move(other.m_file)), m_buf(std::move(other.m_buf)) {}

    // Get the buffer directly
    std::vector<u8>& Buff() { return m_buf; }

    // Read
    struct ReadResponse;
    [[nodiscard]] ReadResponse Read(void* out, u64 size);

    // Close
    struct CloseResponse;
    [[nodiscard]] CloseResponse Close();

private:
    plt::FileDesc m_file;
    std::vector<u8> m_buf;
    // TODO: keep track of current fd position
};

[[nodiscard]] File::ErrorFile CORE_API_EXPORT OpenFile(std::string_view path, u64 flag, u64 mode);
[[nodiscard]] File::Error     CORE_API_EXPORT ReadFileFull(std::string_view path, u64 flag, u64 mode, std::vector<u8>& out);

} // namespace core::fs

namespace core::io
{

using ReadResponse  = core::fs::File::ReadResponse;
using CloseResponse = core::fs::File::CloseResponse;

// Read Interface:
template<> bool                         IsErr(const ReadResponse& res);
template<> core::error::Error           Err(const ReadResponse& res);
template<> u64                          N(const ReadResponse& res);
template<> core::fs::File::ReadResponse Read(core::fs::File& r, void* buf, u64 size);

// Close Interface:
template<> bool                          IsErr(const CloseResponse& res);
template<> core::error::Error            Err(const CloseResponse& res);
template<> core::fs::File::CloseResponse Close(core::fs::File& c);

} // namsepcae core::io
