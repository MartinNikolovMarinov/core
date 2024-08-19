#pragma once

#include "core_alloc.h"
#include "core_API.h"
#include "core_arr.h"
#include "core_expected.h"
#include "core_str_builder.h"
#include "core_traits.h"
#include "core_types.h"
#include "plt/core_path.h"
#include "plt/core_plt_error.h"

// TODO: A few more functions that will be necessary:
//       - fileCopy
//       - fileMove
//       - getCurrWorkingDir
//       - changeCurrWorkingDir
//       - flush a file descriptor to disc (if possible)

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

constexpr const char* fileTypeToCstr(FileType type) {
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

struct DirEntry {
    FileType type;
    const char* name;
};

struct CORE_API_EXPORT FileStat {
    FileType  type;
    addr_size size;

    // TODO: [TIME] Add time for last modification once some time abstraction is in place.
};

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

enum struct SeekMode : u8 {
    Begin,
    Current,
    End
};

using DirWalkCallback = bool (*)(const DirEntry& entry, addr_size idx, void* userData);

CORE_API_EXPORT expected<FileDesc, PltErrCode>  fileOpen(const char* path, OpenMode mode = OpenMode::Default);
CORE_API_EXPORT expected<PltErrCode>            fileClose(FileDesc& file);
CORE_API_EXPORT expected<PltErrCode>            fileDelete(const char* path);
CORE_API_EXPORT expected<PltErrCode>            fileRename(const char* path, const char* newPath);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileWrite(FileDesc& file, const void* in, addr_size size);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileRead(FileDesc& file, void* out, addr_size size);
CORE_API_EXPORT expected<addr_off, PltErrCode>  fileSeek(FileDesc& file, addr_off offset, SeekMode mode = SeekMode::Begin);
CORE_API_EXPORT expected<PltErrCode>            fileStat(const char* path, FileStat& out);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileSize(FileDesc& file);

CORE_API_EXPORT expected<PltErrCode> fileReadEntire(const char* path, ArrList<u8>& out);
CORE_API_EXPORT expected<PltErrCode> fileWriteEntire(const char* path, const u8* data, addr_size size);
CORE_API_EXPORT expected<PltErrCode> fileWriteEntire(const char* path, const ArrList<u8>& in);

CORE_API_EXPORT expected<PltErrCode>       dirCreate(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirDelete(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirDeleteRec(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirRename(const char* path, const char* newPath);
CORE_API_EXPORT expected<PltErrCode>       dirWalk(const char* path, DirWalkCallback cb, void* userData = nullptr);
CORE_API_EXPORT expected<bool, PltErrCode> dirIsEmpty(const char* path);

} // namespace core
