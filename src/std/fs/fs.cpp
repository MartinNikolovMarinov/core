#include <fs.h>

namespace core::fs
{

File::ReadResponse File::Read(void* out, u64 size) {
    File::ReadResponse ret;
    if (size == 0) return ret;
    if (m_buf.empty()) m_buf.resize(File::DEFAULT_BUF_SIZE);

    u64 bytesToRead = std::min(m_buf.size(), size);
    auto rr = plt::OsRead(m_file, m_buf.data(), bytesToRead);
    if (!rr.b.isOk()) {
        ret.m_err = error::Error(std::string("failed to read file reason: ") + strerror(errno));
        ret.m_n = 0;
        return ret;
    }
    i64 readBytes = rr.a;
    if (readBytes == 0) {
        ret.m_err = {ERR_EOF};
        ret.m_n = 0;
        return ret;
    }

    if (out != nullptr) std::memcpy(out, m_buf.data(), readBytes);
    ret.m_err = {};
    ret.m_n = readBytes;
    return ret;
}

File::CloseResponse File::Close() {
    File::CloseResponse ret;
    if (!plt::OsClose(m_file).isOk()) {
        ret.m_err = error::Error(std::string("failed to close file reason: ") + strerror(errno));
        return ret;
    }
    return ret;
}

error::ErrorValue<File> OpenFile(std::string_view path, u64 flag, u64 mode) {
    auto openRes =  plt::OsOpen(path.data(), flag, mode);
    if (!openRes.b.isOk()) {
        return { {}, { std::string("failed to open file: \"") + path.data() + std::string("\"; reason: ") + std::strerror(errno) }};
    }
    plt::FileDesc fd = openRes.a;
    File file(std::move(fd));
    return { std::move(fd), {} };
}

[[nodiscard]] error::Error ReadFileFull(std::string_view path, u64 flag, u64 mode, std::vector<u8>& out) {
    auto openRes = core::fs::OpenFile(path, flag, mode);
    if (openRes.IsErr()) return openRes.Err();

    File file = std::move(openRes.val);
    u8 buf[File::DEFAULT_BUF_SIZE];
    while (true) {
        auto readRes = core::io::Read<File, File::ReadResponse>(file, buf, File::DEFAULT_BUF_SIZE);
        if (readRes.IsErr()) {
            if (readRes.Err().msg == File::ERR_EOF) break;
            return readRes.Err();
        }

        i64 n = readRes.N();
        out.insert(out.end(), buf, buf + n);
    }

    if (auto closeRes = core::io::Close<File, File::CloseResponse>(file); closeRes.IsErr()) {
        return closeRes.Err();
    }

    return {};
}

} // namespace core::fs
