#include <plt/core_fs.h>
#include <plt/core_plt_error.h>
#include <core_arr.h>
#include <core_mem.h>
#include <core_str_builder.h>

namespace core {

expected<PltErrCode> fileReadEntire(const char* path, core::Memory<u8>& out) {
    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Read);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = std::move(res.value());
    }
    defer {
        fileClose(file);
    };

    addr_size size = 0;
    {
        auto res = fileSize(file);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        size = res.value();
    }

    if (size == 0) return {};

    if (out.len() < size) {
        return core::unexpected(ERR_PASSED_BUFFER_TOO_SMALL);
    }

    addr_size readbytes = 0;
    while (readbytes < size) {
        auto res = fileRead(file, out.data() + readbytes, size - readbytes);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        readbytes += res.value();
    }

    return {};
}

expected<PltErrCode> fileReadEntire(const char* path, core::Memory<char>& out) {
    core::Memory<u8> outU8 = { .ptr = reinterpret_cast<u8*>(out.data()), .length = out.len() };
    return fileReadEntire(path, outU8);
}

core::expected<PltErrCode> fileWriteEntire(const char* path, const u8* data, addr_size size) {
    if (size == 0) return {};

    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Write | OpenMode::Create | OpenMode::Truncate);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = std::move(res.value());
    }
    defer {
        fileClose(file);
    };

    addr_size written = 0;
    while (written < size) {
        auto res = fileWrite(file, data + written, size - written);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        written += res.value();
    }

    return {};
}

core::expected<PltErrCode> fileWriteEntire(const char* path, const core::Memory<u8>& in) {
    return fileWriteEntire(path, in.data(), in.len());
}

expected<PltErrCode> fileWriteEntire(const char* path, const Memory<char>& in) {
    core::Memory<u8> inU8 = { .ptr = reinterpret_cast<u8*>(in.data()), .length = in.len() };
    return fileWriteEntire(path, inU8);
}

expected<bool, PltErrCode> dirIsEmpty(const char* path) {
    struct Closure {
        bool isEmpty;
    };

    auto emtpyDirWalk = [](const DirEntry&, addr_size, void* closure) {
        Closure* c = reinterpret_cast<Closure*>(closure);
        c->isEmpty = false;
        return false;
    };

    Closure c = {true};
    auto res = dirWalk(path, emtpyDirWalk, reinterpret_cast<void*>(&c));

    if (res.hasErr()) {
        return core::unexpected(res.err());
    }

    bool ret = c.isEmpty;
    return ret;
}

} // namespace core
