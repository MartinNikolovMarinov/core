#pragma once

#include "core_API.h"
#include "core_types.h"
#include "core_expected.h"
#include "core_utils.h"
#include "plt/core_plt_error.h"

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT FileDesc {
    void* handle; // raw platform handle

    NO_COPY(FileDesc);

    FileDesc();

    FileDesc(FileDesc&& other);
    FileDesc& operator=(FileDesc&& other);

    bool isValid() const;
};

enum struct FileType : u8 {
    None,

    Regular,
    Directory,
    Symlink,
    Other,

    SENTINEL
};

constexpr const char* fileTypeToCptr(FileType type) {
    switch (type) {
        case FileType::None:      return "None";
        case FileType::Regular:   return "Regular";
        case FileType::Directory: return "Directory";
        case FileType::Symlink:   return "Symlink";
        case FileType::Other:     return "Other";
        case FileType::SENTINEL:  return "SENTINEL";
    }

    Assert(false, "Invalid FileType");
    return "None";
}

enum struct OpenMode : u8 {
    Default  = 0,
    Read     = 1 << 0,
    Write    = 1 << 1,
    Append   = 1 << 2,
    Create   = 1 << 3,
    Truncate = 1 << 4,
};

constexpr OpenMode operator|(OpenMode lhs, OpenMode rhs) { return OpenMode(u8(lhs) | u8(rhs)); }
constexpr OpenMode operator|(OpenMode lhs, u8 rhs)       { return OpenMode(u8(lhs) | rhs); }
constexpr OpenMode operator|(u8 lhs, OpenMode rhs)       { return OpenMode(lhs | u8(rhs)); }
constexpr OpenMode operator&(OpenMode lhs, OpenMode rhs) { return OpenMode(u8(lhs) & u8(rhs)); }
constexpr OpenMode operator&(OpenMode lhs, u8 rhs)       { return OpenMode(u8(lhs) & rhs); }
constexpr OpenMode operator&(u8 lhs, OpenMode rhs)       { return OpenMode(lhs & u8(rhs)); }

CORE_API_EXPORT core::expected<FileDesc, PltErrCode> fileOpen(const char* path, OpenMode mode = OpenMode::Default);
CORE_API_EXPORT core::expected<PltErrCode>           fileClose(FileDesc& file);
CORE_API_EXPORT core::expected<PltErrCode>           fileDelete(const char* path);

CORE_API_EXPORT core::expected<PltErrCode> dirCreate(const char* path);
CORE_API_EXPORT core::expected<PltErrCode> dirDelete(const char* path);
CORE_API_EXPORT core::expected<PltErrCode> dirRename(const char* path, const char* newPath);

} // namespace core
