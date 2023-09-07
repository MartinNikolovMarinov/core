#include <API.h>
#include <types.h>
#include <arr.h>
#include <core_traits.h>
#include <expected.h>
#include <std/plt.h>

// FIXME: Replace everything here with a more convinient API. It can be a bit slower this should be a bit higher level
// then the current API.

// TODO: Probably need some thread safety for this code.

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT file_data {
    file_desc fd = {};
    bool isOpen = false;
    // TODO: keep track of file size, current position, etc.
};

static_assert(core::is_standard_layout_v<file_data>);

struct CORE_API_EXPORT file_err {
    enum struct type : u32 {
        ERR_OS = 0,
        ERR_EOF = 1,
        ERR_SHORT_WRITE = 2,
        ERR_FILE_NOT_OPENED = 3,

        SENTINEL
    };

    type err;
    const char* msg; // must be a string literal only.

    file_err() noexcept : err(type::SENTINEL), msg(nullptr) {}
    file_err(type _err, const char* _msg) noexcept : err(_err), msg(_msg) {}

    inline bool is_eof() const { return err == type::ERR_EOF; }
    inline const char* to_cptr() const {
        switch (err) {
            case type::ERR_EOF:             return "end of file";
            case type::ERR_OS:              return "os error";
            case type::ERR_SHORT_WRITE:     return "short write";
            case type::ERR_FILE_NOT_OPENED: return "file not opened";
            case type::SENTINEL:            return "sentinel";
            default:                        return "unknown error";
        }
    }
};

static_assert(core::is_standard_layout_v<file_err>);

CORE_API_EXPORT core::expected<file_err> file_close(file_data& file);

CORE_API_EXPORT core::expected<file_err> file_write(file_data& file,
                                                    const void* in, addr_size size,
                                                    addr_size& writtenBytes,
                                                    addr_size blockSize = core::os_get_default_block_size());

CORE_API_EXPORT expected<file_err> file_read(file_data& file,
                                             void* out, addr_size size,
                                             addr_size& readBytes,
                                             addr_size blockSize = core::os_get_default_block_size());

CORE_API_EXPORT expected<file_data, file_err> file_open(const char* path, i32 flag, i32 mode);

template <typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
expected<core::arr<u8, TAllocator>, file_err> file_read_full(const char* path,
                                                             i32 flag, i32 mode,
                                                             u64 expectedSize = core::os_get_default_block_size()
) {
    auto res = file_open(path, flag, mode);
    if (res.has_err()) return core::unexpected(res.err());

    file_data f = core::move(res.value());
    defer { file_close(f); };

    core::arr<u8, TAllocator> ret (0, expectedSize);
    while (true) {
        addr_size currReadBytes = 0;
        u8 buf[core::os_get_default_block_size()];
        if (expected<file_err> err = file_read(f, buf, core::os_get_default_block_size(), currReadBytes); err.has_err()) {
            if (err.err().is_eof()) {
                ret.append(buf, currReadBytes);
                break;
            }
            return core::unexpected(err.err());
        }

        if (currReadBytes == 0) break;
        ret.append(buf, currReadBytes);
    }

    return ret;
}

} // namespace core
