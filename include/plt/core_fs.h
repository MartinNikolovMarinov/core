#pragma once

#include "core_API.h"
#include "core_types.h"
#include "core_expected.h"
#include "core_utils.h"
#include "core_arr.h"
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

CORE_API_EXPORT core::expected<FileDesc, PltErrCode>  fileOpen(const char* path, OpenMode mode = OpenMode::Default);
CORE_API_EXPORT core::expected<PltErrCode>            fileClose(FileDesc& file);
CORE_API_EXPORT core::expected<PltErrCode>            fileDelete(const char* path);
CORE_API_EXPORT core::expected<PltErrCode>            fileRename(const char* path, const char* newPath);
CORE_API_EXPORT core::expected<addr_size, PltErrCode> fileWrite(FileDesc& file, const void* in, addr_size size);
CORE_API_EXPORT core::expected<addr_size, PltErrCode> fileRead(FileDesc& file, void* out, addr_size size);
CORE_API_EXPORT core::expected<addr_off, PltErrCode>  fileSeek(FileDesc& file, addr_off offset, SeekMode mode = SeekMode::Begin);
CORE_API_EXPORT core::expected<PltErrCode>            fileStat(const char* path, FileStat& out);
CORE_API_EXPORT core::expected<addr_size, PltErrCode> fileSize(FileDesc& file);

CORE_API_EXPORT core::expected<PltErrCode> dirCreate(const char* path);
CORE_API_EXPORT core::expected<PltErrCode> dirDelete(const char* path);
CORE_API_EXPORT core::expected<PltErrCode> dirRename(const char* path, const char* newPath);

// TODO2: [PERFORMANCE] The read and write entire file functions are NOT an attempt on doing performant file I/O!
//        They are just a simple way to read/write a file in one go.
//        At some point I might revisit them, and use some kind of memory mapped file I/O, or at lest do block-sized
//        read/write operations.

template <typename TAlloc>
CORE_API_EXPORT core::expected<PltErrCode> fileReadEntire(const char* path, core::Arr<u8, TAlloc>& out) {
    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Read);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = core::move(res.value());
    }

    out.clear();

    addr_size size = 0;
    {
        auto res = fileSize(file);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        size = res.value();
    }

    if (out.cap() < size) {
        out.adjustCap(size);
    }

    {
        auto res = fileRead(file, out.data(), out.size());
        if (!res) {
            return core::unexpected(res.err());
        }
    }

    return {};
}

template <typename TAlloc>
CORE_API_EXPORT core::expected<PltErrCode> fileWriteEntire(const char* path, const core::Arr<u8, TAlloc>& in) {
    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Write | OpenMode::Create | OpenMode::Truncate);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = core::move(res.value());
    }

    {
        auto res = fileWrite(file, in.data(), in.size());
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
    }

    return {};
}

} // namespace core
