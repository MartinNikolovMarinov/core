#include <plt/core_fs.h>

#include <windows.h>

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

DWORD toDesiredAccess(OpenMode mode) {
    if (i32(mode) == i32(OpenMode::Default)) {
        return GENERIC_READ | GENERIC_WRITE;
    }

    DWORD dwDesiredAccess = 0;

    if (i32(mode) & i32(OpenMode::Read)) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if (i32(mode) & i32(OpenMode::Write)) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    if (i32(mode) & i32(OpenMode::Append)) {
        // Append implies read and write, so just overwrite dwDesiredAccess value.
        dwDesiredAccess = FILE_APPEND_DATA;
    }

    return dwDesiredAccess;
}

DWORD toCreationDisposition(OpenMode mode) {
    DWORD dwDisposition = OPEN_EXISTING;

    if (i32(mode) & i32(OpenMode::Create) &&
        i32(mode) & i32(OpenMode::Truncate)
    ) {
        dwDisposition = CREATE_ALWAYS;
    }
    else if (i32(mode) & i32(OpenMode::Truncate)) {
        dwDisposition = TRUNCATE_EXISTING;
    }
    else if (i32(mode) & i32(OpenMode::Create)) {
        dwDisposition = OPEN_ALWAYS;
    }

    return dwDisposition;
}

} // namespace

core::expected<FileDesc, PltErrCode> fileOpen(const char* path, OpenMode mode) {
    DWORD dwDesiredAccess = toDesiredAccess(mode);
    DWORD dwCreationDisposition = toCreationDisposition(mode);

    // The mode FILE_SHARE_READ allows other processes to ONLY read the file while it is open.
    DWORD sharingMode = FILE_SHARE_READ;

    HANDLE handle = CreateFile(path, dwDesiredAccess, sharingMode, nullptr,
                               dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (handle == INVALID_HANDLE_VALUE) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    FileDesc fd;
    fd.handle = reinterpret_cast<void*>(handle);
    return {};
}

core::expected<PltErrCode> fileClose(FileDesc& file) {
    if (!CloseHandle(file.handle)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    file.handle = nullptr;
    return {};
}

core::expected<PltErrCode> fileDelete(const char* path) {
    if (!DeleteFile(path)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}

core::expected<PltErrCode> dirCreate(const char* path) {
    if (!dirCreateectory(path, nullptr)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}

core::expected<PltErrCode> dirDelete(const char* path) {
    if (!RemoveDirectory(path)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}

core::expected<PltErrCode> dirRename(const char* path, const char* newPath) {
    if (!MoveFile(path, newPath)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}


} // namespace core
