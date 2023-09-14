#include <API.h>
#include <types.h>
#include <arr.h>
#include <io.h>
#include <core_traits.h>
#include <expected.h>
#include <std/plt.h>

namespace core {

using namespace coretypes;

constexpr const char* FS_ERR_FILE_NOT_OPENED = "file not opened";
constexpr const char* FS_SHORT_WRITE = "short write";
constexpr const char* FS_ERR_READING_DIRECTORY = "reading directory";
constexpr const char* FS_ERR_WRITING_DIRECTORY = "writing directory";
constexpr const char* FS_ERR_SEEKING_DIRECTORY = "seeking directory";

struct CORE_API_EXPORT file : public ireader_writer_closer, public iseeker {
    file() : m_fd({}), m_offset(0), m_isOpen(false), m_isDirectory(false) {}

    inline bool is_open() const { return m_isOpen; }
    inline bool is_directory() const { return m_isDirectory; }
    inline const file_desc fd() const { return m_fd; }
    inline addr_off offset() const { return m_offset; }

    virtual core::expected<addr_size, io_err> read(void* out, addr_size size) override;
    virtual core::expected<addr_size, io_err> write(const void* in, addr_size size) override;
    virtual core::expected<io_err> close() override;
    virtual core::expected<io_err> seek(addr_off offset, seek_origin origin) override;

    core::expected<io_err> take_desc(file_desc&& fd, addr_off offset = 0, const file_stat* stat = nullptr);

private:
    file_desc m_fd;
    addr_off m_offset;
    bool m_isOpen;
    bool m_isDirectory;

    // FIXME: Use mutex!
};

template <typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
core::expected<io_err> file_read_full(const char* path, core::arr<char, TAllocator>& out) {
    auto openRes = core::os_open(path, core::default_file_params());
    if (openRes.has_err()) {
        return core::unexpected(io_err { false, core::os_get_err_cptr(openRes.err()) });
    }

    auto fdesec = openRes.value();
    core::file f;
    if (auto takeRes = f.take_desc(core::move(fdesec)); takeRes.has_err()) {
        return core::unexpected(takeRes.err());
    }

    const addr_size blockSize = core::os_get_default_block_size();
    char buf[blockSize];
    while (true) {
        auto readRes = f.read(buf, blockSize);
        if (readRes.has_err()) {
            if (readRes.err().eof) break;
            return core::unexpected(readRes.err());
        }

        if (readRes.value() == 0) break;
        out.append(buf, readRes.value());
    }

    if (auto closeRes = f.close(); closeRes.has_err()) {
        return core::unexpected(closeRes.err());
    }

    return {};
}

} // namespace core
