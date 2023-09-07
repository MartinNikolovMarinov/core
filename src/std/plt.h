#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>
#include <system_checks.h>
#include <io.h>

#include <limits.h>

namespace core {

using namespace coretypes;

using plt_err_code = i64;

namespace detail {
constexpr plt_err_code OS_CUSTOM_ERROR_START = 0xffffffff00000000; // Custom library errors start from this value.
}

constexpr plt_err_code OS_DEALLOC_NULL_ADDR_ERR = detail::OS_CUSTOM_ERROR_START + 1;

constexpr u32 MAX_FILE_LENGTH = NAME_MAX;
constexpr u32 MAX_PATH_LENGTH = PATH_MAX;

CORE_API_EXPORT expected<void*, plt_err_code> os_alloc_pages(addr_size size);
CORE_API_EXPORT expected<plt_err_code>        os_dealloc_pages(void *addr, addr_size size);

CORE_API_EXPORT expected<u64, plt_err_code> os_unix_time_stamp_in_ms();
CORE_API_EXPORT expected<plt_err_code>      os_thread_sleep(u64 ms);

// file descriptor
struct CORE_API_EXPORT file_desc {
    // Holds the OS specific file descriptor.
    void* desc;

    file_desc();
    file_desc(void* desc);
    u64 to_u64();
};

enum struct CORE_API_EXPORT file_type : u64 {
    Unknown,
    Regular,
    Directory,
    Device,
    NamedPipe,
    Symlink,
    Socket,
    UnixCharDevice,

    SENTINEL
};

constexpr const char* file_type_to_cptr(file_type type) {
    switch (type) {
        case file_type::Regular:        return "Regular";
        case file_type::Directory:      return "Directory";
        case file_type::Device:         return "Device";
        case file_type::NamedPipe:      return "Named Pipe";
        case file_type::Symlink:        return "Symlink";
        case file_type::Socket:         return "Socket";
        case file_type::UnixCharDevice: return "Unix Char Device";

        case file_type::SENTINEL: [[fallthrough]];
        case file_type::Unknown:  return "Unknown";
    }

    return "Invalid";
}

// 4KB block size is a good default for most operating and files system.
// However, this api allows chages to the default block size, if for some reason it is required.
// API is NOT thread safe!
CORE_API_EXPORT void os_set_default_block_size(addr_size size);
CORE_API_EXPORT addr_size os_get_default_block_size();

struct CORE_API_EXPORT dir_entry {
    file_type type;
    const char* name;
};

enum CORE_API_EXPORT file_access : u32 {
    FA_None = 0,
    FA_Read = 1 << 0,
    FA_Write = 1 << 1,
    FA_Execute = 1 << 2,
    FA_ReadWrite = FA_Read | FA_Write,
    FA_ReadExecute = FA_Read | FA_Execute,
    FA_WriteExecute = FA_Write | FA_Execute,
    FA_All = FA_Read | FA_Write | FA_Execute
};

constexpr const char* file_access_to_cptr(file_access fa) {
    switch (fa) {
        case FA_None:         return "None";
        case FA_Read:         return "Read";
        case FA_Write:        return "Write";
        case FA_Execute:      return "Execute";
        case FA_ReadWrite:    return "Read Write";
        case FA_ReadExecute:  return "Read Execute";
        case FA_WriteExecute: return "Write Execute";
        case FA_All:          return "Read Write Execute";
    }
    return "Invalid";
}

struct CORE_API_EXPORT file_access_group {
    file_access user;
    file_access group;
    file_access other;
};

CORE_API_EXPORT file_access_group default_file_access_group();
CORE_API_EXPORT file_access_group default_dir_access_group();

enum CORE_API_EXPORT file_flags : u32 {
    FF_None = 0,
    FF_ReadOnly = 1 << 0,
    FF_WriteOnly = 1 << 1,
    FF_ReadWrite = FF_ReadOnly | FF_WriteOnly,
    FF_Append = 1 << 2,
    FF_Create = 1 << 3,
    FF_Trunc = 1 << 4
};

CORE_API_EXPORT file_flags default_file_flags();

struct CORE_API_EXPORT file_params {
    file_access_group access;
    file_flags flags;
    i32 osSpecificFlags;
};

CORE_API_EXPORT file_params default_file_params();

struct CORE_API_EXPORT file_stat {
    u64 mtime;
    addr_size size;
    addr_size blksize;
    file_type type;
    file_access_group access;

    bool isDir() const { return type == file_type::Directory; }
};

CORE_API_EXPORT expected<file_desc, plt_err_code> os_open(const char* path, const file_params& params = default_file_params());
CORE_API_EXPORT expected<file_desc, plt_err_code> os_create(const char* path, const file_access_group& access = default_file_access_group());
CORE_API_EXPORT expected<file_desc, plt_err_code> os_opendir(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead);
CORE_API_EXPORT expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten);
CORE_API_EXPORT expected<plt_err_code> os_close(file_desc fd);
CORE_API_EXPORT expected<plt_err_code> os_rmfile(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_mkdir(const char* path, const file_access_group& access = default_dir_access_group());
CORE_API_EXPORT expected<plt_err_code> os_rmdir(const char* path);
CORE_API_EXPORT expected<bool, plt_err_code> os_exists(const char* path);
CORE_API_EXPORT expected<file_stat, plt_err_code> os_stat(const char* path);
CORE_API_EXPORT expected<file_stat, plt_err_code> os_fstat(file_desc fd);
CORE_API_EXPORT expected<addr_off, plt_err_code> os_seek(file_desc fd, addr_off offset, seek_origin origin);

template <typename TWalkerFn>
expected<plt_err_code> os_dir_walk(file_desc fd, TWalkerFn cb);

using AtExitCb = void (*)(i32 exitCode);

CORE_API_EXPORT void os_exit(i32 exitCode);
CORE_API_EXPORT void at_exit(AtExitCb atExit);

CORE_API_EXPORT const char* os_get_err_cptr(plt_err_code err);

} // namespace core

// Import the OS specific headers:

#if OS_LINUX == 1
#include <std/plt/unix/unix_plt.h>
#elif OS_MAC == 1
#include <std/plt/unix/unix_plt.h>
#endif
