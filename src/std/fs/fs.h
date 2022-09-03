#pragma once

#include <types.h>
#include <core_error.h>
#include <plt.h>
#include <io.h>

#include <cstring>
#include <vector>

namespace core::fs
{

using namespace coretypes;

struct File {
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
    struct ReadResponse {
        ReadResponse() : m_n(0), m_err(error::Error()) {}

        bool IsErr() const { return m_err.IsErr(); }
        error::Error Err() const { return m_err; }
        i64 N() const { return m_n; }

    private:
        friend struct File;
        i64 m_n;
        error::Error m_err;
    };

    [[nodiscard]] ReadResponse Read(void* out, u64 size);

    // Close
    struct CloseResponse {
        CloseResponse() : m_err(error::Error()) {}

        bool IsErr() const { return m_err.IsErr(); }
        error::Error Err() const { return m_err; }

    private:
        friend struct File;
        error::Error m_err;
    };

    [[nodiscard]] CloseResponse Close();

private:
    plt::FileDesc m_file;
    std::vector<u8> m_buf;
    // TODO: keep track of current fd position
};

[[nodiscard]] error::ErrorValue<File> OpenFile(std::string_view path, u64 flag, u64 mode);
[[nodiscard]] error::Error ReadFileFull(std::string_view path, u64 flag, u64 mode, std::vector<u8>& out);

} // namespace core::fs

namespace core::io
{

template<> bool         IsErr(const core::fs::File::ReadResponse& res)  { return res.IsErr(); }
template<> error::Error Err(const core::fs::File::ReadResponse& res)    { return res.Err(); }
template<> u64          N(const core::fs::File::ReadResponse& res)      { return res.N(); }

template<> core::fs::File::ReadResponse Read(core::fs::File& r, void* buf, u64 size) { return r.Read(buf, size); }

template<> core::fs::File::CloseResponse Close(core::fs::File& c) { return c.Close(); }

} // namsepcae core::io
