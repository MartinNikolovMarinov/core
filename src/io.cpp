#include <io.h>

namespace core {

core::expected<io_err> read_full(ireader& reader, void* out, addr_size size) {
    addr_size read = 0;
    while (read < size) {
        auto res = reader.read(reinterpret_cast<u8*>(out) + read, size - read);
        if (res.has_err()) {
            return core::unexpected(res.err());
        }
        read += res.value();
    }
    return { };
}

core::expected<io_err> write_full(iwriter& writer, const void* in, addr_size size) {
    addr_size written = 0;
    while (written < size) {
        auto res = writer.write(reinterpret_cast<const u8*>(in) + written, size - written);
        if (res.has_err()) {
            return core::unexpected(res.err());
        }
        written += res.value();
    }
    return { };
}

} // namespace core

