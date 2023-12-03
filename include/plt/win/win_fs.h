#pragma once

#include <core_cptr.h>
#include <plt/core_fs.h>

#include <windows.h>

namespace core {

namespace detail {

DirEntry toDirEntry(WIN32_FIND_DATAA& findData) {
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

} // namespace detail

template <typename TCallback>
core::expected<PltErrCode> dirWalk(const char* path, TCallback cb) {
    WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(path, &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return core::unexpected(PltErrCode(GetLastError()));
    }

    PltErrCode errCode = 0;
    addr_size i = 0;

    while (true) {
        DirEntry de = detail::toDirEntry(findData);

        bool shouldSkip = core::cptrEqual(de.name, ".") || core::cptrEqual(de.name, "..");

        if (!shouldSkip) {
            if (!cb(de, i)) {
                break;
            }
            i++;
        }

        if (!FindNextFileA(findHandle, &findData)) {
            if (GetLastError() != ERROR_NO_MORE_FILES) {
                errCode = PltErrCode(GetLastError());
            }

            break;
        }
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
