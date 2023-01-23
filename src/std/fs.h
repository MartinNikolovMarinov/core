#pragma once

#include <API.h>
#include <arr.h>
#include <expected.h>
#include <types.h>
#include <std/plt.h>

#include <string>
#include <string_view>
#include <cstring>

namespace core {

// TODO: might want to get this from the platform layer at some point.
constexpr static ptr_size FS_DEFAULT_BLOCK_SIZE = 4096;

template<ptr_size TBlockSize = FS_DEFAULT_BLOCK_SIZE>
struct CORE_API_EXPORT file {
    struct file_error {
        // NOTE: This error is quite costly but I will take that downsite to have helpful instantly obvious error messages.
        enum struct type : u32 {
            ERR_NONE = 0,
            ERR_EOF = 1,
            ERR_PARTIAL_WRITE = 2,
            ERR_OS = 3
        };
        type err = type::ERR_NONE;
        std::string errMsg;
        inline bool is_ok()            const { return err == type::ERR_NONE; }
        inline bool is_eof()           const { return err == type::ERR_EOF; }
        inline bool is_partial_write() const { return err == type::ERR_PARTIAL_WRITE; }
        static const char* to_cptr(type err) noexcept {
            switch (err) {
                case type::ERR_NONE:          return "no error";
                case type::ERR_EOF:           return "end of file";
                case type::ERR_PARTIAL_WRITE: return "partial write";
                case type::ERR_OS:            return "os error";
                default:                      return "unknown error";
            }
        }
    };
    using err_type = file_error;
    using buffer_type = u8;
    static constexpr ptr_size blockSize = TBlockSize;

    file() = default;

    // no copy
    file(const file&) = delete;
    file& operator=(const file&) = delete;

    // move
    file(file&& other) noexcept : m_fd(other.m_fd) { other.m_fd = {}; }
    file& operator=(file&& other) noexcept {
        m_fd = other.m_fd;
        other.m_fd = {};
        return *this;
    }

    [[nodiscard]] static expected<err_type> rmdir(file& f) noexcept {
        if (auto err = core::os_rmdir(f.m_name.data()); err.has_err()) {
            err_type res = {
                err_type::type::ERR_OS,
                std::string("failed to delete directory file reason: ") + strerror(err.err())
            };
            return unexpected(res);
        }
        return {};
    }

    [[nodiscard]] static expected<err_type> rmfile(file& f) noexcept {
        if (auto err = core::os_rmfile(f.m_name.data()); err.has_err()) {
            err_type res = {
                err_type::type::ERR_OS,
                std::string("failed to delete file reason: ") + strerror(err.err())
            };
            return unexpected(res);
        }
        return {};
    }

    [[nodiscard]] static expected<file, err_type> open_file(std::string&& path, u64 flag, u64 mode) {
        file f;
        if (auto ores = f.open(core::move(path), flag, mode); ores.has_err()) {
            return unexpected(ores.err());
        }
        return f;
    }

    [[nodiscard]] const std::string& name() { return m_name; }

    [[nodiscard]] expected<err_type> open(std::string&& path, u64 flag, u64 mode) {
        auto ores = core::os_open(path.data(), flag, mode);
        if (ores.has_err()) {
            auto errMsg = std::string("failed to open file: \"") + path.data() +
                          std::string("\"; reason: ") + std::strerror(ores.err());
            return unexpected(err_type { err_type::type::ERR_OS, errMsg });
        }
        m_fd = ores.value();
        m_name = core::move(path);
        return {};
    }

    [[nodiscard]] bool is_open() const noexcept { return m_fd.desc != nullptr; }

    struct read_write_response {
        err_type error;
        ptr_size bytesCount;

        bool has_err() const noexcept   { return !error.is_ok(); }
        bool is_eof() const noexcept    { return error.is_eof(); }
        err_type& err() noexcept        { return error; }
        ptr_size value() const noexcept { return bytesCount; }
    };

    [[nodiscard]] read_write_response read(void* out, ptr_size size) noexcept {
        read_write_response res;
        if (size == 0) return res;
        if (out == nullptr) return res;

        // FIXME: implement this to read in chunks to exactly size of bytes.
        //        also be careful to tests this, because the implementation is kinda clunky!

        return res;
    }

    [[nodiscard]] read_write_response write(const void* in, ptr_size size) noexcept {
        read_write_response res;
        if (size == 0) return res;
        if (in == nullptr) return res;

        // FIXME: implement this to write in chunks to exactly size of bytes.
        //        also be careful to tests this, because the implementation is kinda clunky!

        return res;
    }

    [[nodiscard]] expected<err_type> close() noexcept {
        if (!is_open()) return {};
        if (auto err = core::os_close(m_fd); err.has_err()) {
            err_type res = {
                err_type::type::ERR_OS,
                std::string("failed to close file reason: ") + strerror(err.err())
            };
            return unexpected(res);
        }
        m_fd = {};
        return {};
    }

private:
    file(file_desc fd) noexcept : m_fd(fd) {}

    file_desc m_fd;
    std::string m_name;
    buffer_type m_buf[blockSize];

    // TODO: Keep track of current fd position to allow seeking.
};

// [[nodiscard]] static err_type    read_full(std::string_view path, u64 flag, u64 mode, data_buffer_type& out);
// [[nodiscard]] static err_type    read_full(data_buffer_type& out);
// [[nodiscard]] static err_type    write_full(std::string_view path, u64 flag, u64 mode, const data_buffer_type& in);
// [[nodiscard]] static err_type    write_full(const data_buffer_type& in);

} // namespace core
