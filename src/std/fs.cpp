#include <core_math.h>
#include <std/fs.h>

namespace core {

core::expected<file_err> file_close(file_data& file) {
    if (file.isOpen) {
        if (expected<plt_err_code> err = core::os_close(file.fd); err.has_err()) {
            const char* errCptr = core::os_get_err_cptr(err.err());
            return core::unexpected(file_err { file_err::type::ERR_OS, errCptr });
        }
        file = file_data{};
    }
    return {};
}

core::expected<file_err> file_write(file_data& file,
                                    const void* in, addr_size size,
                                    addr_size& writtenBytes,
                                    addr_size blockSize
) {
    if (size == 0) return {};
    if (in == nullptr) return {};
    if (file.isOpen == false) {
        return core::unexpected(file_err { file_err::type::ERR_FILE_NOT_OPENED,
                                           "trying to write to a file that is not open" });
    }

    addr_size chunkSize = core::min(size, blockSize);
    while (true) {
        addr_off currWritenBytes = 0;
        if (expected<plt_err_code> err = core::os_write(file.fd, in, chunkSize, currWritenBytes); err.has_err()) {
            const char* errCptr = core::os_get_err_cptr(err.err());
            return core::unexpected(file_err { file_err::type::ERR_OS, errCptr });
        }

        writtenBytes += addr_size(currWritenBytes);
        if (addr_size(currWritenBytes) != chunkSize) {
            // Might mean that the OS is out of space, but that should be returned as an error in the above call.
            // TODO: Verify assumption!
            return core::unexpected(file_err { file_err::type::ERR_SHORT_WRITE, "might be out of space on disc" });
        }
        Assert(writtenBytes >= size, "[BUG] wrote more than the requested size");
        if (writtenBytes == size) break;
        if (writtenBytes + chunkSize > size) {
            // next chunk will be the last one and it might be smaller than the block size.
            chunkSize = size - writtenBytes;
        }
    }

    return {};
}

expected<file_err> file_read(file_data& file,
                             void* out, addr_size size,
                             addr_size& readBytes,
                             addr_size blockSize
) {
    if (size == 0) return {};
    if (out == nullptr) return {};
    if (file.isOpen == false) {
        return core::unexpected(file_err { file_err::type::ERR_FILE_NOT_OPENED,
                                           "trying to read from a file that is not open" });
    }

    const addr_size chunkSize = core::min(size, blockSize);
    while (true) {
        addr_off currReadBytes = 0;
        if (expected<plt_err_code> err = core::os_read(file.fd, out, chunkSize, currReadBytes); err.has_err()) {
            const char* errCptr = core::os_get_err_cptr(err.err());
            return core::unexpected(file_err { file_err::type::ERR_OS, errCptr });
        }

        if (currReadBytes == 0) {
            return core::unexpected(file_err { file_err::type::ERR_EOF, {} });
        }

        readBytes += addr_size(currReadBytes);
        Assert(readBytes <= size, "[BUG] read more than the requested size");
        if (readBytes == size) break;
    }

    return {};
}

expected<file_data, file_err> file_open(const char* path, i32 flag, i32 mode) {
    // TODO: I should create an abstraction for flag and mode which is cross platform.
    //       After that I should remove flag and mode from this function declaration.
    //       It simply does not make sense to have them here.
    //       I do it just because this file should not leak os specific stuff.

    expected<file_desc, plt_err_code> fd = core::os_open(path, flag, mode);
    if (fd.has_err()) {
        const char* errCptr = core::os_get_err_cptr(fd.err());
        return core::unexpected(file_err { file_err::type::ERR_OS, errCptr });
    }
    file_data f;
    f.fd = fd.value();
    f.isOpen = true;
    return f;
}

} // namespace core
