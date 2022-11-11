#include <fs.h>

#include <cstring>

namespace core::fs
{

File::FileResponse File::Read(void* out, u64 size) {
    File::FileResponse ret;
    if (size == 0) return ret;

    u8 buf[m_bufSize];
    i64 bytesToRead = m_bufSize < size ? m_bufSize : size;
    auto rr = plt::OsRead(m_file, buf, bytesToRead);
    if (rr.err.IsErr()) {
        ret.m_err = error::Error(std::string("failed to read file reason: ") + strerror(rr.err.Err()));
        ret.m_n = 0;
        return ret;
    }

    i64 readBytes = rr.val;
    if (readBytes == 0) {
        ret.m_err = { File::ERR_EOF };
        ret.m_n = 0;
        return ret;
    }

    if (out != nullptr) core::MemCopy(out, buf, readBytes);
    ret.m_err = {};
    ret.m_n = readBytes;
    return ret;
}

File::FileResponse File::Write(const void* in, u64 size) {
    File::FileResponse ret;
    if (size == 0) return ret;
    if (in == nullptr) return ret;

    u8 buf[m_bufSize];
    i64 bytesToWrite = m_bufSize < size ? m_bufSize : size;
    core::MemCopy(buf, in, bytesToWrite);
    auto wr = plt::OsWrite(m_file, buf, bytesToWrite);
    if (wr.err.IsErr()) {
        ret.m_err = error::Error(std::string("failed to write file reason: ") + strerror(wr.err.Err()));
        ret.m_n = 0;
        return ret;
    }

    i64 writtenBytes = wr.val;
    if (writtenBytes != bytesToWrite) {
        ret.m_err = { File::ERR_PARTIAL_WRITE };
        ret.m_n = writtenBytes;
        return ret;
    }

    ret.m_err = {};
    ret.m_n = writtenBytes;
    return ret;
}

File::FileResponse File::Close() {
    File::FileResponse ret;
    if (auto err = plt::OsClose(m_file); err.IsErr()) {
        ret.m_err = error::Error(std::string("failed to close file reason: ") + strerror(err.Err()));
        return ret;
    }
    return ret;
}

File::ErrorFile OpenFile(std::string_view path, u64 flag, u64 mode) {
    auto openRes =  plt::OsOpen(path.data(), flag, mode);
    if (openRes.err.IsErr()) {
        auto errMsg =  std::string("failed to open file: \"") + path.data() +
                       std::string("\"; reason: ") + std::strerror(openRes.err.Err());
        return { {}, { errMsg }};
    }
    plt::FileDesc fd = openRes.val;
    File file(std::move(fd));
    return { std::move(file), {} };
}

File::Error ReadFileFull(std::string_view path, u64 flag, u64 mode, File::DataBuffer& out) {
    auto openRes = core::fs::OpenFile(path, flag, mode);
    if (openRes.err.IsErr()) return openRes.err;
    File file = std::move(openRes.val);
    return ReadFileFull(file, out);
}

File::Error WriteFileFull(std::string_view path, u64 flag, u64 mode, const File::DataBuffer& in) {
    auto openRes = core::fs::OpenFile(path, flag, mode);
    if (openRes.err.IsErr()) return openRes.err;
    if (in.empty()) return {};
    File file = std::move(openRes.val);
    return WriteFileFull(file, in);
}

File::Error ReadFileFull(File& file, File::DataBuffer& out) {
    u8 buf[file.m_bufSize];
    while (true) {
        auto readRes = core::Read(file, buf, file.m_bufSize);
        if (core::IsErr(readRes)) {
            core::error::Error readErr = core::Err(readRes);
            if (readErr.Err() == File::ERR_EOF) break;
            return readErr;
        }

        i64 n = core::N(readRes);
        out.insert(out.end(), buf, buf + n);
    }

    if (auto closeRes = core::Close(file); core::IsErr(closeRes)) {
        return core::Err(closeRes);
    }

    return {};
}

File::Error WriteFileFull(File& file, const File::DataBuffer& in) {
    auto it = in.begin();
    while(true) {
        auto size = std::distance(it, in.end());
        auto writeRes = core::Write(file, it.base(), size);
        if (core::IsErr(writeRes)) {
            core::error::Error writeErr = core::Err(writeRes);
            return writeErr;
        }

        i64 n = core::N(writeRes);
        Assert(it.base() + n > in.end().base(), "BUG: wrote more bytes than expected");
        it += n;
    }
    return {};
}

} // namespace core::fs
