#include <plt/core_fs.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace core {

FileDesc::FileDesc() : handle(nullptr) {}

FileDesc::FileDesc(FileDesc&& other) {
    handle = other.handle;
    other.handle = nullptr;
}

FileDesc& FileDesc::operator=(FileDesc&& other) {
    handle = other.handle;
    other.handle = nullptr;
    return *this;
}

bool FileDesc::isValid() const {
    return handle != nullptr;
}

namespace {

inline void* toHandle(i32 fd) { return reinterpret_cast<void*>(fd); }

inline i32 fromHandle(void* handle) {
    // Some casting magic to confuse pedantic compilers.
    i32 ret = static_cast<i32>(reinterpret_cast<uintptr_t>(handle));
    return ret;
}

constexpr inline i32 toOsFlags(OpenMode m) {
    if (m == OpenMode::Default) {
        return O_RDWR;
    }

    i32 flag = 0;

    if (i8(m) & i8(OpenMode::Read) && i8(m) & i8(OpenMode::Write)) {
        flag |= O_RDWR;
    }
    else if (i8(m) & i8(OpenMode::Read)) {
        flag |= O_RDONLY;
    }
    else if (i8(m) & i8(OpenMode::Write)) {
        flag |= O_WRONLY;
    }

    if (i8(m) & i8(OpenMode::Truncate)) {
        flag |= O_TRUNC;
    }
    else if (i8(m) & i8(OpenMode::Append)) {
        flag |= O_APPEND;
    }

    if (i8(m) & i8(OpenMode::Create)) {
        flag |= O_CREAT;
    }

    return flag;
}

// Default access mode for files is "-rw-rw-r--"
static constexpr mode_t DEFAULT_FILE_ACCESS_MODE = (S_IRUSR | S_IWUSR) |
                                                   (S_IRGRP | S_IWGRP) |
                                                   (S_IROTH);

// Default access mode for directories is "drwxrwxr-x"
static constexpr mode_t DEFAULT_DIR_ACCESS_MODE = (S_IRUSR | S_IWUSR | S_IXUSR) |
                                                  (S_IRGRP | S_IWGRP | S_IXGRP) |
                                                  (S_IROTH | S_IXOTH);

} // namespace

core::expected<FileDesc, PltErrCode> fileOpen(const char* path, OpenMode openMode) {
    i32 flags = toOsFlags(openMode);
    mode_t accessMode = DEFAULT_FILE_ACCESS_MODE;

    i32 res = open(path, flags, accessMode);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    FileDesc fd;
    fd.handle = toHandle(res);
    return fd;
}

core::expected<PltErrCode> fileClose(FileDesc& file) {
    if (!file.isValid()) {
        return core::unexpected(core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
    }

    i32 res = close(fromHandle(file.handle));
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    file.handle = nullptr;
    return {};
}

core::expected<PltErrCode> fileDelete(const char* path) {
    i32 res = unlink(path);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> fileRename(const char* path, const char* newPath) {
    i32 res = rename(path, newPath);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<addr_size, PltErrCode> fileWrite(FileDesc& file, const void* in, addr_size size) {
    if (!file.isValid()) {
        return core::unexpected(core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
    }

    auto res = write(fromHandle(file.handle), in, size);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return addr_size(res);
}

core::expected<addr_size, PltErrCode> fileRead(FileDesc& file, void* out, addr_size size) {
    if (!file.isValid()) {
        return core::unexpected(core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
    }

    auto res = read(fromHandle(file.handle), out, size);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return addr_size(res);
}

core::expected<addr_off, PltErrCode> fileSeek(FileDesc& file, addr_off offset, SeekMode seekMode) {
    if (!file.isValid()) {
        return core::unexpected(core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
    }

    i32 whence = 0;
    switch (seekMode) {
        case SeekMode::Begin:   whence = SEEK_SET; break;
        case SeekMode::Current: whence = SEEK_CUR; break;
        case SeekMode::End:     whence = SEEK_END; break;
    }

    auto res = lseek(fromHandle(file.handle), offset, whence);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return addr_off(res);
}

core::expected<PltErrCode> fileStat(const char* path, FileStat& out) {
    struct stat statbuf;
    i32 res = stat(path, &statbuf);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    out.size = addr_size(statbuf.st_size);

    if (S_ISDIR(statbuf.st_mode)) {
        out.type = FileType::Directory;
    }
    else if (S_ISREG(statbuf.st_mode)) {
        out.type = FileType::Regular;
    }
    else if (S_ISLNK(statbuf.st_mode)) {
        out.type = FileType::Symlink;
    }
    else {
        out.type = FileType::Other;
    }

    return {};
}

core::expected<addr_size, PltErrCode> fileSize(FileDesc& file) {
    if (!file.isValid()) {
        return core::unexpected(core::ERR_PASSED_INVALID_FILE_DESCRIPTOR);
    }

    struct stat statbuf;
    i32 res = fstat(fromHandle(file.handle), &statbuf);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return addr_size(statbuf.st_size);
}

core::expected<PltErrCode> dirCreate(const char* path) {
    mode_t accessMode = DEFAULT_DIR_ACCESS_MODE;

    i32 res = mkdir(path, accessMode);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> dirDelete(const char* path) {
    i32 res = rmdir(path);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> dirRename(const char* path, const char* newPath) {
    i32 res = rename(path, newPath);
    if (res < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

namespace {

inline DirEntry toDirEntry(const dirent& d) {
    DirEntry de = {};
    de.name = d.d_name; // d.d_name should be null terminated according to documentation.
    switch (d.d_type) {
        case DT_REG:
            de.type = FileType::Regular;
            break;
        case DT_DIR:
            de.type = FileType::Directory;
            break;
        case DT_LNK:
            de.type = FileType::Symlink;
            break;

        case DT_FIFO:
        case DT_SOCK:
        case DT_CHR:
        case DT_BLK:
        default:
            de.type = FileType::Other;
    }

    return de;
}

};

core::expected<PltErrCode> dirWalk(const char* path, DirWalkCallback cb, void* userData) {
    if (path == nullptr || cb == nullptr) {
        return core::unexpected(PltErrCode(EINVAL));
    }

    DIR* dir = opendir(path);
    if (!dir) {
        return core::unexpected(PltErrCode(errno));
    }

    PltErrCode errCode = 0;
    struct dirent* entry;
    addr_size i = 0;

    errno = 0;

    while (true) {
        entry = readdir(dir);
        if (!entry) {
            // reached the last directory entry.
            if (errno != 0) {
                // or an error occurred.
                errCode = PltErrCode(errno);
            }

            break;
        }

        bool shouldSkip = (core::cptrCmp(entry->d_name, core::cptrLen(entry->d_name), ".", 1) == 0) ||
                          (core::cptrCmp(entry->d_name, core::cptrLen(entry->d_name), "..", 2) == 0);
        if (shouldSkip) {
            continue;
        }

        DirEntry de = toDirEntry(*entry);
        if (!cb(de, i, userData)) break;
        i++;
    }

    if (closedir(dir) < 0) {
        return core::unexpected(PltErrCode(errno));
    }

    if (errCode != 0) {
        return core::unexpected(errCode);
    }

    return {};
}

} // namespace core
