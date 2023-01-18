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

template<typename TAllocator>
struct CORE_API_EXPORT file {
    using err_type = std::string;
    using data_buffer_type = core::arr<u8, TAllocator>;

    constexpr static std::string_view ERR_EOF = "end of file";
    constexpr static std::string_view ERR_PARTIAL_WRITE = "partial write";
    constexpr static ptr_size DEFAULT_BUFFER_SIZE = 4096;

    const ptr_size bufferSize;

    file() noexcept : bufferSize(DEFAULT_BUFFER_SIZE) {}

    // no copy
    file(const file&) = delete;
    file& operator=(const file&) = delete;

    // move
    file(file&& other) noexcept : bufferSize(other.bufferSize), m_fd(other.m_fd) { other.m_fd = {}; }
    file& operator=(file&& other) noexcept {
        m_fd = other.m_fd;
        other.m_fd = {};
        return *this;
    }


    [[nodiscard]] static expected<err_type> rmdir(file& f) noexcept {
        if (auto err = core::os_rmdir(f.m_name.data()); err.has_err()) {
            return unexpected(std::string("failed to delete directory file reason: ") + strerror(err.err()));
        }
        return {};
    }

    [[nodiscard]] static expected<err_type> rmfile(file& f) noexcept {
        if (auto err = core::os_rmfile(f.m_name.data()); err.has_err()) {
            return unexpected(std::string("failed to delete file reason: ") + strerror(err.err()));
        }
        return {};
    }

    [[nodiscard]] static expected<file, err_type> open_file(std::string&& path, u64 flag, u64 mode) {
        file f;
        if (auto ores = f.open(std::move(path), flag, mode); ores.has_err()) {
            return unexpected(std::move(ores.err()));
        }
        return f;
    }

    [[nodiscard]] expected<err_type> open(std::string&& path, u64 flag, u64 mode) {
        auto ores = core::os_open(path.data(), flag, mode);
        if (ores.has_err()) {
            auto errMsg = std::string("failed to open file: \"") + path.data() +
                          std::string("\"; reason: ") + std::strerror(ores.err());
            return unexpected(std::move(errMsg));
        }
        m_fd = ores.value();
        m_name = std::move(path);
        return {};
    }

    [[nodiscard]] bool is_open() const noexcept { return m_fd.desc != nullptr; }

    struct read_write_response {
        err_type error;
        ptr_size bytesCount;

        bool has_err() const noexcept   { return !error.empty(); }
        bool is_eof() const noexcept    { return error == file::ERR_EOF; } // FIXME: should compare some enum here.
        err_type& err() noexcept        { return error; }
        ptr_size value() const noexcept { return bytesCount; }
    };

    [[nodiscard]] read_write_response read(void* out, ptr_size size) noexcept {
        read_write_response res;
        if (size == 0) return res;

        u8 buf[bufferSize];
        ptr_size bytesToRead = bufferSize < size ? bufferSize : size;
        if (auto rres = core::os_read(m_fd, buf, bytesToRead, res.bytesCount); rres.has_err()) {
            return { std::string("failed to read file reason: ") + strerror(rres.err()), 0 };
        }

        if (res.bytesCount == 0) {
            res.error = ERR_EOF;
            return res;
        }

        if (out != nullptr) core::memcopy(out, buf, res.bytesCount);
        return res;
    }

    [[nodiscard]] read_write_response write(const void* in, ptr_size size) noexcept {
        read_write_response res;
        if (size == 0) return res;
        if (in == nullptr) return res;

        u8 buf[bufferSize];
        ptr_size bytesToWrite = bufferSize < size ? bufferSize : size;
        core::memcopy(buf, in, bytesToWrite);
        if (auto wres = core::os_write(m_fd, buf, bytesToWrite, res.bytesCount); wres.has_err()) {
            return { std::string("failed to write file reason: ") + strerror(wres.err()), 0 };
        }

        if (res.bytesCount != bytesToWrite) {
            res.error = ERR_EOF;
            return res;
        }

        return res;
    }

    [[nodiscard]] expected<err_type> close() noexcept {
        if (!is_open()) return {};
        if (auto err = core::os_close(m_fd); err.has_err()) {
            return unexpected(std::string("failed to close file reason: ") + strerror(err.err()));
        }
        m_fd = {};
        return {};
    }

private:
    file(file_desc fd) noexcept : bufferSize(DEFAULT_BUFFER_SIZE), m_fd(fd) {}
    file(file_desc fd, ptr_size bufferSize) noexcept : bufferSize(bufferSize), m_fd(fd) {}

    file_desc m_fd;
    std::string m_name;
    // TODO: keep track of current fd position
};


// [[nodiscard]] static err_type    read_full(std::string_view path, u64 flag, u64 mode, data_buffer_type& out);
// [[nodiscard]] static err_type    read_full(data_buffer_type& out);
// [[nodiscard]] static err_type    write_full(std::string_view path, u64 flag, u64 mode, const data_buffer_type& in);
// [[nodiscard]] static err_type    write_full(const data_buffer_type& in);

} // namespace core
