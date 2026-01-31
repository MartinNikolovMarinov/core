#pragma once

#include "core_API.h"
#include "core_expected.h"
#include "core_mem.h"
#include "core_types.h"

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
    NONE,

    Regular,
    Directory,
    Symlink,
    Other,

    SENTINEL
};

constexpr const char* fileTypeToCstr(FileType type) {
    switch (type) {
        case FileType::NONE:      return "None";
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

    // TODO: Add last modification timestamp.
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
CORE_API_EXPORT expected<PltErrCode>            fileMove(const char* path, const char* newPath);
CORE_API_EXPORT expected<PltErrCode>            fileCopy(const char* from, const char* to);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileWrite(FileDesc& file, const void* in, addr_size size);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileRead(FileDesc& file, void* out, addr_size size);
CORE_API_EXPORT expected<PltErrCode>            fileTruncate(const char* path, addr_size length);
CORE_API_EXPORT expected<PltErrCode>            fileTruncate(FileDesc& file, addr_size length);
CORE_API_EXPORT expected<addr_off, PltErrCode>  fileSeek(FileDesc& file, addr_off offset, SeekMode mode = SeekMode::Begin);
CORE_API_EXPORT expected<bool, PltErrCode>      fileExists(const char* path);
CORE_API_EXPORT expected<PltErrCode>            fileStat(const char* path, FileStat& out);
CORE_API_EXPORT expected<PltErrCode>            fileStat(FileDesc& file, FileStat& out);
CORE_API_EXPORT expected<addr_size, PltErrCode> fileSize(FileDesc& file);
CORE_API_EXPORT expected<PltErrCode>            fileFlush(FileDesc& file);

CORE_API_EXPORT expected<PltErrCode> fileReadEntire(const char* path, Memory<u8>& out);
CORE_API_EXPORT expected<PltErrCode> fileReadEntire(const char* path, Memory<char>& out);
CORE_API_EXPORT expected<PltErrCode> fileWriteEntire(const char* path, const u8* data, addr_size size);
CORE_API_EXPORT expected<PltErrCode> fileWriteEntire(const char* path, const Memory<u8>& in);
CORE_API_EXPORT expected<PltErrCode> fileWriteEntire(const char* path, const Memory<char>& in);

CORE_API_EXPORT expected<PltErrCode>       dirCreate(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirDelete(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirWalk(const char* path, DirWalkCallback cb, void* userData = nullptr);
CORE_API_EXPORT expected<bool, PltErrCode> dirIsEmpty(const char* path);
CORE_API_EXPORT expected<PltErrCode>       dirCWD(char* out, addr_size size);
CORE_API_EXPORT expected<PltErrCode>       dirChangeCWD(const char* path);

} // namespace core
