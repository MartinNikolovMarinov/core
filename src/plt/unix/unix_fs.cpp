#include <plt/core_fs.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

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

constexpr inline i32 toOsFlags(OpenMode mode) {
    if (mode == OpenMode::Default) {
        return O_RDWR;
    }

    i32 flag = 0;

    if (i8(mode) & i8(OpenMode::Read) && i8(mode) & i8(OpenMode::Write)) {
        flag |= O_RDWR;
    }
    else if (i8(mode) & i8(OpenMode::Read)) {
        flag |= O_RDONLY;
    }
    else if (i8(mode) & i8(OpenMode::Write)) {
        flag |= O_WRONLY;
    }

    if (i8(mode) & i8(OpenMode::Truncate)) {
        flag |= O_TRUNC;
    }
    else if (i8(mode) & i8(OpenMode::Append)) {
        flag |= O_APPEND;
    }

    if (i8(mode) & i8(OpenMode::Create)) {
        flag |= O_CREAT;
    }

    return flag;
}

// Default access mode for files is "-rw-rw-r--"
static constexpr mode_t DEFAULT_FILE_MODE = (S_IRUSR | S_IWUSR) |
                                            (S_IRGRP | S_IWGRP) |
                                            (S_IROTH);

// Default access mode for directories is "drwxrwxr-x"
static constexpr mode_t DEFAULT_DIR_MODE = (S_IRUSR | S_IWUSR | S_IXUSR) |
                                           (S_IRGRP | S_IWGRP | S_IXGRP) |
                                           (S_IROTH | S_IXOTH);

} // namespace

core::expected<FileDesc, PltErrCode> fileOpen(const char* path, OpenMode openMode) {
    i32 flags = toOsFlags(openMode);
    mode_t mode = DEFAULT_FILE_MODE;

    i32 res = open(path, flags, mode);
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    FileDesc fd;
    fd.handle = toHandle(res);
    return fd;
}

core::expected<PltErrCode> fileClose(FileDesc& file) {
    i32 res = close(fromHandle(file.handle));
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    file.handle = nullptr;
    return {};
}

core::expected<PltErrCode> fileDelete(const char* path) {
    i32 res = unlink(path);
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> dirCreate(const char* path) {
    mode_t mode = DEFAULT_DIR_MODE;

    i32 res = mkdir(path, mode);
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> dirDelete(const char* path) {
    i32 res = rmdir(path);
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

core::expected<PltErrCode> dirRename(const char* path, const char* newPath) {
    i32 res = rename(path, newPath);
    if (res == -1) {
        return core::unexpected(PltErrCode(errno));
    }

    return {};
}

} // namespace core
