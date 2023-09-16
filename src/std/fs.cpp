#include <core_math.h>
#include <std/fs.h>

namespace core {

namespace {

io_err create_io_err_from_plt(plt_err_code pltErrCode) {
    return io_err { false, core::os_get_err_cptr(pltErrCode) };
}

} // namespace


core::expected<addr_size, io_err> file::write(const void* in, addr_size size) {
    if (size == 0) return 0;
    if (in == nullptr) return 0;
    if (m_isOpen == false) {
        return core::unexpected(io_err { false, FS_ERR_FILE_NOT_OPENED });
    }
    if (m_isDirectory) {
        return core::unexpected(io_err { false, FS_ERR_WRITING_DIRECTORY });
    }

    addr_off chunkSize = addr_off(core::min(size, core::os_get_default_block_size()));
    addr_off written = 0;
    if (auto res = core::os_write(m_fd, in, u64(chunkSize), written); res.has_err()) {
        return core::unexpected(create_io_err_from_plt(res.err()));
    }

    m_offset += written;

    if (written != chunkSize) {
        // Might mean that the OS is out of space, but that should be returned as an error in the above call.
        return core::unexpected(io_err { false, FS_SHORT_WRITE });
    }

    return addr_size(written);
}

core::expected<addr_size, io_err> file::read(void* out, addr_size size) {
    if (size == 0) return 0;
    if (out == nullptr) return 0;
    if (m_isOpen == false) {
        return core::unexpected(io_err { false, FS_ERR_FILE_NOT_OPENED });
    }
    if (m_isDirectory) {
        return core::unexpected(io_err { false, FS_ERR_READING_DIRECTORY });
    }

    const addr_size chunkSize = core::min(size, core::os_get_default_block_size());
    addr_off readBytes = 0;
    if (auto res = core::os_read(m_fd, out, chunkSize, readBytes); res.has_err()) {
        return core::unexpected(create_io_err_from_plt(res.err()));
    }

    if (readBytes == 0) {
        io_err eofErr = { true, nullptr };
        return core::unexpected(eofErr);
    }

    m_offset += readBytes;

    return addr_size(readBytes);
}

core::expected<io_err> file::close() {
    if (m_isOpen) {
        if (auto res = core::os_close(m_fd); res.has_err()) {
            return core::unexpected(create_io_err_from_plt(res.err()));
        }
        m_fd = {};
        m_isOpen = false;
        m_offset = 0;
    }
    return {};
}

core::expected<io_err> file::seek(addr_off offset, seek_origin origin) {
    if (m_isOpen == false) {
        return core::unexpected(io_err { false, FS_ERR_FILE_NOT_OPENED });
    }
    if (m_isDirectory) {
        return core::unexpected(io_err { false, FS_ERR_SEEKING_DIRECTORY });
    }

    auto res = core::os_seek(m_fd, offset, origin);
    if (res.has_err()) {
        return core::unexpected(create_io_err_from_plt(res.err()));
    }

    m_offset = res.value();
    return {};
}

core::expected<io_err> file::take_desc(file_desc&& fd, addr_off offset, const file_stat* stat) {
    m_fd = fd;
    m_isOpen = true;
    m_offset = offset;
    fd = file_desc{};

    if (stat == nullptr) {
        auto res = core::os_fstat(m_fd);
        if (res.has_err()) {
            return core::unexpected(create_io_err_from_plt(res.err()));
        }
        stat = &res.value();
    }

    m_isDirectory = stat->isDir();
    return {};
}

} // namespace core
