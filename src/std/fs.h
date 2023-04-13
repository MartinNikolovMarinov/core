#pragma once

#include <types.h>
#include <expected.h>
#include <mem.h>
#include <std/plt.h>

#include <string>

namespace core {

using namespace coretypes;

constexpr static ptr_size FS_DEFAULT_BLOCK_SIZE = 4096;

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
struct File {
    struct Error {
        // NOTE: This error is quite costly but I will take that downsite to have helpful instantly obvious error messages.
        enum struct Type : u32 {
            ERR_OS = 0,
            ERR_EOF = 1,
            ERR_SHORT_WRITE = 2,
            ERR_FILE_NOT_OPENED = 3,

            SENTINEL
        };
        Type err = Type::SENTINEL;
        std::string errMsg;
        inline bool isEOF() const { return err == Type::ERR_EOF; }
        inline const char* toCptr() const {
            switch (err) {
                case Type::ERR_EOF:             return "end of file";
                case Type::ERR_OS:              return "os error";
                case Type::ERR_SHORT_WRITE:     return "short write";
                case Type::ERR_FILE_NOT_OPENED: return "file not opened";
                default:                        return "unknown error";
            }
        }
    };

    using BufferType = u8;

    File() = default;
    File(file_desc&& fd, std::string&& name)
        : m_fd(core::move(fd))
        , m_name(core::move(name))
        , m_isOpen(true) {}

    // no copy
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // move
    File(File&& other) noexcept
        : m_fd(other.m_fd)
        , m_name(core::move(other.m_name))
        , m_isOpen(other.m_isOpen) {
            other.m_fd = {};
            other.m_isOpen = false;
            core::memcopy(m_buffer, other.m_buffer, TBlockSize);
        }

    File& operator=(File&& other) noexcept {
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

    expected<Error> close() {
        defer {
            m_fd = {};
            m_name = {};
            m_isOpen = false;
        };
        if (auto err = core::os_close(m_fd); err.has_err()) {
            Error res = {
                Error::Type::ERR_OS,
                std::string("failed to close file reason: ") + core::os_getErrCptr(err.err())
            };
            return core::unexpected(res);
        }
        return {};
    }

    expected<Error> write(const void* in, ptr_size size, ptr_size& writtenBytes) {
        if (size == 0) return {};
        if (in == nullptr) return {};
        if (m_isOpen == false) return core::unexpected(Error {Error::Type::ERR_FILE_NOT_OPENED, "file is not open"});

        ptr_size chunkSize = (size > TBlockSize) ? TBlockSize : size;
        while(true) {
            ptr_size currWrite = 0;
            if (auto wres = core::os_write(m_fd, in, chunkSize, currWrite); wres.has_err()) {
                Error err = {
                    Error::Type::ERR_OS,
                    std::string("failed to write file reason: ") + core::os_getErrCptr(wres.err())
                };
                return core::unexpected(err);
            }
            writtenBytes += currWrite;
            if (currWrite != chunkSize) {
                // might mean that the OS is out of space, but that should be returned as an error in the above call.
                // TODO: verify assumption
                Error err = { Error::Type::ERR_SHORT_WRITE, {} };
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

    expected<Error> read(void* out, ptr_size size, ptr_size& readBytes) {
        if (size == 0) return {};
        if (out == nullptr) return {};
        if (m_isOpen == false) return core::unexpected(Error {Error::Type::ERR_FILE_NOT_OPENED, "file is not open"});

        const ptr_size chunkSize = (size > TBlockSize) ? TBlockSize : size;
        while(true) {
            ptr_size currRead = 0;
            if (auto rres = core::os_read(m_fd, out, chunkSize, currRead); rres.has_err()) {
                Error err = {
                    Error::Type::ERR_OS,
                    std::string("failed to read file reason: ") + core::os_getErrCptr(rres.err())
                };
                return core::unexpected(err);
            }
            readBytes += currRead;
            if (currRead == 0) {
                Error err = { Error::Type::ERR_EOF, {} };
                return core::unexpected(err);
            }
        }

        return {};
    }

private:
    file_desc m_fd;
    std::string m_name;
    bool m_isOpen = false;
    BufferType m_buffer[TBlockSize];
};

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<typename File<TBlockSize>::Error> rmdir(File<TBlockSize>& f) {
    using Error = typename File<TBlockSize>::Error;

    if (auto err = core::os_rmdir(f.m_name.data()); err.has_err()) {
        Error res = {
            Error::Type::ERR_OS,
            std::string("failed to delete directory file reason: ") + core::os_getErrCptr(err.err())
        };
        return core::unexpected(res);
    }
    return {};
}

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<typename File<TBlockSize>::Error> rmfile(File<TBlockSize>& f) {
    using Error = typename File<TBlockSize>::Error;

    if (auto err = core::os_rmfile(f.name().data()); err.has_err()) {
        Error res = {
            Error::Type::ERR_OS,
            std::string("failed to delete file reason: ") + core::os_getErrCptr(err.err())
        };
        return core::unexpected(res);
    }
    return {};
}

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
expected<File<TBlockSize>, typename File<TBlockSize>::Error> openfile(const char* path, u64 flag, u64 mode) {
    using Error = typename File<TBlockSize>::Error;

    auto res = core::os_open(path, flag, mode);
    if (res.has_err()) {
        Error err = {
            Error::Type::ERR_OS,
            std::string("failed to open file reason: ") + core::os_getErrCptr(res.err())
        };
        return core::unexpected(err);
    }
    auto fd = res.value();
    File<TBlockSize> f (core::move(fd), std::string(path));
    return f;
}

} // namespace core
