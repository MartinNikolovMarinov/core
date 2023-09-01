#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>
#include <system_checks.h>

#include <limits.h>

namespace core {

using namespace coretypes;

using plt_err_code = i64;

constexpr plt_err_code OS_DEALLOC_NULL_ADDR_ERR = 1;

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

enum struct dir_entry_type : u64 {
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

constexpr const char* dir_entry_type_to_cptr(dir_entry_type type) {
    switch (type) {
        case dir_entry_type::Regular:        return "Regular";
        case dir_entry_type::Directory:      return "Directory";
        case dir_entry_type::Device:         return "Device";
        case dir_entry_type::NamedPipe:      return "NamedPipe";
        case dir_entry_type::Symlink:        return "Symlink";
        case dir_entry_type::Socket:         return "Socket";
        case dir_entry_type::UnixCharDevice: return "UnixCharDevice";

        case dir_entry_type::SENTINEL: [[fallthrough]];
        case dir_entry_type::Unknown:  return "Unknown";
    }

    return "Invalid";
}

// 4KB block size is a good default for most operating and files system.
// However, this api allows chages to the default block size, if for some reason it is required.
// API is NOT thread safe!
CORE_API_EXPORT void os_set_default_block_size(addr_size size);
CORE_API_EXPORT addr_size os_get_default_block_size();

struct CORE_API_EXPORT dir_entry {
    dir_entry_type type;
    const char* name;
};

CORE_API_EXPORT expected<file_desc, plt_err_code> os_open(const char* path, i32 flag, i32 mode);
CORE_API_EXPORT expected<file_desc, plt_err_code> os_opendir(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead);
CORE_API_EXPORT expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten);
CORE_API_EXPORT expected<plt_err_code> os_close(file_desc fd);
CORE_API_EXPORT expected<plt_err_code> os_rmfile(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_mkdir(const char* path, i32 mode);
CORE_API_EXPORT expected<plt_err_code> os_rmdir(const char* path);
CORE_API_EXPORT expected<plt_err_code> os_stat(const char* path);

// TODO: Stat information should conform to the following interface:
// type FileInfo interface {
// 	Name() const char*  // base name of the file
// 	Size() addr_size    // length in bytes for regular files; system-dependent for others
// 	Type() FileType     // file mode bits // This is the type of file
// 	ModTime() Time ?    // modification timestamp, maybe dir_entry_type could work here too ?
// 	IsDir() bool        // is directory
// 	Sys() any           // underlying data source (can return nil)
// }

template <typename TWalkerFn>
expected<plt_err_code> os_dir_walk(file_desc fd, TWalkerFn cb);

using AtExitCb = void (*)(i32 exitCode);

CORE_API_EXPORT void os_exit(i32 exitCode);
CORE_API_EXPORT void at_exit(AtExitCb atExit);

CORE_API_EXPORT expected<bool, plt_err_code> os_exists(const char* path);
CORE_API_EXPORT const char* os_get_err_cptr(plt_err_code err);

} // namespace core

// Import the OS specific headers:

#if OS_LINUX == 1
#include <std/plt/unix/unix_plt.h>
#elif OS_MAC == 1
#include <std/plt/unix/unix_plt.h>
#endif
