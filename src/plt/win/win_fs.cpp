#include <plt/core_fs.h>

#include <windows.h>

// TODO2: It's important to note that this API does not support Unicode paths!
//       This is because a unicode compliant implementations must use the wide character versions of the Windows API.
//       This would require a conversion from char* to wchar_t* which is too expensive and is not necessary for 99% of
//       use cases. I should, however, support a setting that allows unicode support.

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
    if (i32(mode) & i32(OpenMode::Truncate)) {
        // Truncate implies write.
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

void convertCharPtrToWCharPtr(const char* src, wchar_t* dst, size_t size) {
    [[maybe_unused]] size_t ignored = 0;
    mbstowcs_s(&ignored, dst, size, src, size);
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
    return fd;
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

core::expected<PltErrCode> fileRename(const char* path, const char* newPath) {
    if (!MoveFile(path, newPath)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return {};
}

core::expected<addr_size, PltErrCode> fileWrite(FileDesc& file, const void* in, addr_size size) {
    DWORD bytesWritten = 0;
    if (!WriteFile(reinterpret_cast<HANDLE>(file.handle), in, DWORD(size), &bytesWritten, nullptr)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return addr_size(bytesWritten);
}

core::expected<addr_size, PltErrCode> fileRead(FileDesc& file, void* out, addr_size size) {
    DWORD bytesRead = 0;
    if (!ReadFile(reinterpret_cast<HANDLE>(file.handle), out, DWORD(size), &bytesRead, nullptr)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    return addr_size(bytesRead);
}

core::expected<addr_off, PltErrCode> fileSeek(FileDesc& file, addr_off offset, SeekMode mode) {
    DWORD dwMoveMethod = 0;
    switch (mode) {
        case SeekMode::Begin:   dwMoveMethod = FILE_BEGIN;   break;
        case SeekMode::Current: dwMoveMethod = FILE_CURRENT; break;
        case SeekMode::End:     dwMoveMethod = FILE_END;     break;
    }

    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = offset;

    LARGE_INTEGER liNewFilePointer;
    if (!SetFilePointerEx(reinterpret_cast<HANDLE>(file.handle), liDistanceToMove, &liNewFilePointer, dwMoveMethod)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    return addr_off(liNewFilePointer.QuadPart);
}

core::expected<PltErrCode> fileStat(const char* path, FileStat& out) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &data)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    out.size = (u64(data.nFileSizeHigh) << 32) | data.nFileSizeLow;

    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        out.type = FileType::Directory;
    }
    else if (data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
        out.type = FileType::Regular;
    }
    else if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        out.type = FileType::Symlink;
    }
    else {
        out.type = FileType::Other;
    }

    return {};
}

core::expected<addr_size, PltErrCode> fileSize(FileDesc& file) {
    LARGE_INTEGER liFileSize;
    if (!GetFileSizeEx(reinterpret_cast<HANDLE>(file.handle), &liFileSize)) {
        return core::unexpected(PltErrCode(GetLastError()));
    }
    addr_size res = addr_size(liFileSize.QuadPart);
    return res;
}

core::expected<PltErrCode> dirCreate(const char* path) {
    if (!CreateDirectory(path, nullptr)) {
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

namespace {

inline DirEntry toDirEntry(const WIN32_FIND_DATAA& findData) {
    DirEntry de = {};
    de.name = findData.cFileName;
    switch (findData.dwFileAttributes) {
        case FILE_ATTRIBUTE_NORMAL:
        case FILE_ATTRIBUTE_ARCHIVE:
        case FILE_ATTRIBUTE_COMPRESSED:
        case FILE_ATTRIBUTE_DEVICE:
        case FILE_ATTRIBUTE_ENCRYPTED:
        case FILE_ATTRIBUTE_HIDDEN:
        case FILE_ATTRIBUTE_INTEGRITY_STREAM:
        case FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:
        case FILE_ATTRIBUTE_NO_SCRUB_DATA:
        case FILE_ATTRIBUTE_OFFLINE:
        case FILE_ATTRIBUTE_READONLY:
        case FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS:
        case FILE_ATTRIBUTE_RECALL_ON_OPEN:
        case FILE_ATTRIBUTE_SPARSE_FILE:
        case FILE_ATTRIBUTE_SYSTEM:
        case FILE_ATTRIBUTE_TEMPORARY:
        case FILE_ATTRIBUTE_VIRTUAL:
            de.type = FileType::Regular;
            break;
        case FILE_ATTRIBUTE_DIRECTORY:
            de.type = FileType::Directory;
            break;
        case FILE_ATTRIBUTE_REPARSE_POINT:
            de.type = FileType::Symlink;
            break;

        default:
            de.type = FileType::Other;
    }

    return de;
}

} // namespace

expected<PltErrCode> dirWalk(const char* path, DirWalkCallback cb, void* userData) {
    using core::StrBuilder;

    StrBuilder pathSb(core::sv(path));
    if (pathSb.empty()) {
        return core::unexpected(PltErrCode(EINVAL));
    }

    pathSb.append("/*"_sv);

    WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(pathSb.view().data(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    PltErrCode errCode = 0;
    addr_size i = 0;

    while (true) {
        if (!FindNextFileA(findHandle, &findData)) {
            if (GetLastError() != ERROR_NO_MORE_FILES) {
                errCode = PltErrCode(GetLastError());
            }

            break;
        }

        bool shouldSkip = core::cptrEq(findData.cFileName, ".", 1) || core::cptrEq(findData.cFileName, "..", 2);
        if (shouldSkip) {
            continue;
        }

        DirEntry de = toDirEntry(findData);
        if (!cb(de, i, userData)) break;
        i++;
    }

    if (FindClose(findHandle) <= 0) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    if (errCode != 0) {
        return core::unexpected(errCode);
    }

    return {};
}

} // namespace core
