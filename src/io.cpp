#include <io.h>

namespace core {

core::expected<io_err> read_nbytes(ireader& reader, void* out, addr_size n) {
    addr_size read = 0;
    while (read < n) {
        auto res = reader.read(reinterpret_cast<u8*>(out) + read, n - read);
        if (res.has_err()) {
            return core::unexpected(res.err());
        }
        read += res.value();
    }
    return { };
}

core::expected<io_err> write_nbytes(iwriter& writer, const void* in, addr_size n) {
    addr_size written = 0;
    while (written < n) {
        auto res = writer.write(reinterpret_cast<const u8*>(in) + written, n - written);
        if (res.has_err()) {
            return core::unexpected(res.err());
        }
        written += res.value();
    }
    return { };
}

} // namespace core

