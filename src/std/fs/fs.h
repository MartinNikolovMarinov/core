#pragma once

#include <API.h>
#include <core.h>
#include <core_error.h>
#include <plt.h>

#include <vector>
#include <string_view>

namespace core::fs
{

using namespace coretypes;

struct CORE_API_EXPORT File {
    using Error      = core::error::Error;
    using ErrorFile  = core::error::ErrorValue<File, Error>;
    using DataBuffer = std::vector<u8>;

    struct FileResponse {
        friend File;

        FileResponse() : m_n(0), m_err(Error()) {}

        i64   N()     const { return m_n; }
        bool  IsErr() const { return m_err.IsErr(); }
        Error Err()   const { return m_err; }

    private:
        i64 m_n;
        Error m_err;
    };

    constexpr static i64 DEFAULT_BUFFER_SIZE = 4096;

    static constexpr std::string_view ERR_EOF = "EOF";
    static constexpr std::string_view ERR_PARTIAL_WRITE = "Partial Write";

    File() : m_file({}), m_bufSize(DEFAULT_BUFFER_SIZE) {}
    File(plt::FileDesc&& file) : m_file(std::move(file)), m_bufSize(DEFAULT_BUFFER_SIZE) {}
    File(plt::FileDesc&& file, i64 bufSize) : m_file(std::move(file)), m_bufSize(bufSize) {}

    // Allow move
    File(File&& other) : m_file(std::move(other.m_file)), m_bufSize(other.m_bufSize) {}

    // Prevent copy
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;

    [[nodiscard]] FileResponse Read(void* out, u64 size)       noexcept;
    [[nodiscard]] FileResponse Close()                         noexcept;
    [[nodiscard]] FileResponse Write(const void* in, u64 size) noexcept;

private:
    friend File::ErrorFile OpenFile(std::string_view path, u64 flag, u64 mode);
    friend File::Error     ReadFileFull(std::string_view path, u64 flag, u64 mode, File::DataBuffer& out);
    friend File::Error     ReadFileFull(File& file, File::DataBuffer& out);
    friend File::Error     WriteFileFull(std::string_view path, u64 flag, u64 mode, const File::DataBuffer& in);
    friend File::Error     WriteFileFull(File& file, const File::DataBuffer& in);

    plt::FileDesc m_file;
    const u32 m_bufSize;
    // TODO: keep track of current fd position
};

[[nodiscard]] File::ErrorFile CORE_API_EXPORT OpenFile(std::string_view path, u64 flag, u64 mode);
[[nodiscard]] File::Error     CORE_API_EXPORT ReadFileFull(std::string_view path, u64 flag, u64 mode, File::DataBuffer& out);
[[nodiscard]] File::Error     CORE_API_EXPORT ReadFileFull(File& file, File::DataBuffer& out);
[[nodiscard]] File::Error     CORE_API_EXPORT WriteFileFull(std::string_view path, u64 flag, u64 mode, const File::DataBuffer& in);
[[nodiscard]] File::Error     CORE_API_EXPORT WriteFileFull(File& file, const File::DataBuffer& in);

} // namespace core::fs
