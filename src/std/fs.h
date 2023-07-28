#pragma once

#include <types.h>
#include <expected.h>
#include <mem.h>
#include <arr.h>
#include <core_traits.h>
#include <std/plt.h>

#include <string>

namespace core {

using namespace coretypes;

constexpr static ptr_size FS_DEFAULT_BLOCK_SIZE = 4096;

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
struct file {
    struct file_err {
        // NOTE: This error is quite costly but I will take that downsite to have helpful instantly obvious error messages.
        enum struct type : u32 {
            ERR_OS = 0,
            ERR_EOF = 1,
            ERR_SHORT_WRITE = 2,
            ERR_FILE_NOT_OPENED = 3,

            SENTINEL
        };
        type err = type::SENTINEL;
        std::string errMsg;
        inline bool is_eof() const { return err == type::ERR_EOF; }
        inline const char* to_cptr() const {
            switch (err) {
                case type::ERR_EOF:             return "end of file";
                case type::ERR_OS:              return "os error";
                case type::ERR_SHORT_WRITE:     return "short write";
                case type::ERR_FILE_NOT_OPENED: return "file not opened";
                default:                        return "unknown error";
            }
        }
    };

    using BufferType = u8;

    file() = default;
    file(file_desc&& fd, std::string&& name)
        : m_fd(core::move(fd))
        , m_name(core::move(name))
        , m_isOpen(true) {}

    // no copy
    file(const file&) = delete;
    file& operator=(const file&) = delete;

    // move
    file(file&& other) noexcept
        : m_fd(other.m_fd)
        , m_name(core::move(other.m_name))
        , m_isOpen(other.m_isOpen) {
            other.m_fd = {};
            other.m_isOpen = false;
            core::memcopy(m_buffer, other.m_buffer, TBlockSize);
        }

    file& operator=(file&& other) noexcept {
        m_fd = other.m_fd;
        m_name = core::move(other.m_name);
        m_isOpen = other.m_isOpen;
        core::memcopy(m_buffer, other.m_buffer, TBlockSize);
        other.m_fd = {};
        other.m_isOpen = false;
        return *this;
    }

    // getters
    bool isOpen() const { return m_isOpen; }
    const std::string& name() const { return m_name; }
    const file_desc& fd() const { return m_fd; }

    expected<file_err> close() {
        defer {
            m_fd = {};
            m_name = {};
            m_isOpen = false;
        };
        if (auto err = core::os_close(m_fd); err.has_err()) {
            file_err res = {
                file_err::type::ERR_OS,
                std::string("failed to close file reason: ") + core::os_get_err_cptr(err.err())
            };
            return core::unexpected(res);
        }
        return {};
    }

    expected<file_err> write(const void* in, ptr_size size, ptr_size& writtenBytes) {
        if (size == 0) return {};
        if (in == nullptr) return {};
        if (m_isOpen == false) return core::unexpected(file_err {file_err::type::ERR_FILE_NOT_OPENED, "file is not open"});

        ptr_size chunkSize = (size > TBlockSize) ? TBlockSize : size;
        while(true) {
            ptr_size currWrite = 0;
            if (auto wres = core::os_write(m_fd, in, chunkSize, currWrite); wres.has_err()) {
                file_err err = {
                    file_err::type::ERR_OS,
                    std::string("failed to write file reason: ") + core::os_get_err_cptr(wres.err())
                };
                return core::unexpected(err);
            }
            writtenBytes += currWrite;
            if (currWrite != chunkSize) {
                // might mean that the OS is out of space, but that should be returned as an error in the above call.
                // TODO: verify assumption
                file_err err = { file_err::type::ERR_SHORT_WRITE, {} };
                return core::unexpected(err);
            }
            Assert(writtenBytes >= size, "[BUG] read past the specified size");
            if (writtenBytes == size) break;
            if (writtenBytes + chunkSize > size) {
                // next chunk will be the last one and it might be smaller than the block size.
                chunkSize = size - writtenBytes;
            }
        }

        return {};
    }

    expected<file_err> read(void* out, ptr_size size, ptr_size& readBytes) {
        if (size == 0) return {};
        if (out == nullptr) return {};
        if (m_isOpen == false) return core::unexpected(file_err {file_err::type::ERR_FILE_NOT_OPENED, "file is not open"});

        const ptr_size chunkSize = (size > TBlockSize) ? TBlockSize : size;
        while(true) {
            ptr_size currRead = 0;
            if (auto rres = core::os_read(m_fd, out, chunkSize, currRead); rres.has_err()) {
                file_err err = {
                    file_err::type::ERR_OS,
                    std::string("failed to read file reason: ") + core::os_get_err_cptr(rres.err())
                };
                return core::unexpected(err);
            }

            if (currRead == 0) {
                file_err err = { file_err::type::ERR_EOF, {} };
                return core::unexpected(err);
            }

            readBytes += currRead;
            if (readBytes >= size) break;
        }

        return {};
    }

private:
    // TODO: Keep track of the file descriptor possition in order to be able to implement seeking.
    file_desc m_fd;
    std::string m_name;
    bool m_isOpen = false;
    BufferType m_buffer[TBlockSize];
};

static_assert(core::is_standard_layout_v<file<FS_DEFAULT_BLOCK_SIZE>>, "file should be standard layout");

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<typename file<TBlockSize>::file_err> rmdir(file<TBlockSize>& f) {
    using Error = typename file<TBlockSize>::file_err;

    if (auto err = core::os_rmdir(f.m_name.data()); err.has_err()) {
        Error res = {
            Error::type::ERR_OS,
            std::string("failed to delete directory file reason: ") + core::os_get_err_cptr(err.err())
        };
        return core::unexpected(res);
    }
    return {};
}

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<typename file<TBlockSize>::file_err> rmfile(file<TBlockSize>& f) {
    using Error = typename file<TBlockSize>::file_err;

    if (auto err = core::os_rmfile(f.name().data()); err.has_err()) {
        Error res = {
            Error::type::ERR_OS,
            std::string("failed to delete file reason: ") + core::os_get_err_cptr(err.err())
        };
        return core::unexpected(res);
    }
    return {};
}

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<file<TBlockSize>, typename file<TBlockSize>::file_err> open_file(const char* path, u64 flag, u64 mode) {
    using Error = typename file<TBlockSize>::file_err;

    auto res = core::os_open(path, flag, mode);
    if (res.has_err()) {
        Error err = {
            Error::type::ERR_OS,
            std::string("failed to open file reason: ") + core::os_get_err_cptr(res.err())
        };
        return core::unexpected(err);
    }
    auto fd = res.value();
    file<TBlockSize> f (core::move(fd), std::string(path));
    return f;
}

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE, typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
expected<core::arr<u8, TAllocator>, typename file<TBlockSize>::file_err> read_full(const char* path, u64 flag, u64 mode, u64 initBufferSize = FS_DEFAULT_BLOCK_SIZE) {
    // TODO: I should create an abstraction for flag and mode which is cross platform.
    //       After that I should remove flag and mode from this function declaration.
    //       It simply does not make sense to have them here.
    //       I do it just because this file should not leak os specific stuff.

    auto res = core::open_file<TBlockSize>(path, flag, mode);
    if (res.has_err()) return core::unexpected(res.err());

    file<TBlockSize> f = core::move(res.value());
    defer { f.close(); };

    core::arr<u8, TAllocator> ret(0, initBufferSize);
    while(true) {
        ptr_size currReadBytes = 0;
        u8 buffer[TBlockSize];
        if (auto err = f.read(buffer, TBlockSize, currReadBytes); err.has_err()) {
            if (err.err().is_eof()) {
                ret.append(buffer, currReadBytes);
                break;
            }
            return core::unexpected(err.err());
        }
        if (currReadBytes == 0) break;
        ret.append(buffer, currReadBytes);
    }

    return ret;
}

} // namespace core
