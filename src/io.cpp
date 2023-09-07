#include <io.h>

namespace core {

core::expected<io_err> write_full(iwriter& writer, void* in, addr_size size) {
    addr_size written = 0;
    while (written < size) {
        auto res = writer.write(reinterpret_cast<char*>(in) + written, size - written);
        if (res.has_err()) {
            return core::unexpected(res.err());
        }
        written += res.value();
    }
    return { };
}

} // namespace core

