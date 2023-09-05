#include <std/plt/unix/unix_plt.h>

namespace core {

file_desc::file_desc()           : desc((void*)(-1)) {}
file_desc::file_desc(void* desc) : desc(desc) {}

u64 file_desc::to_u64() { return (u64)desc; }

expected<void*, plt_err_code> os_alloc_pages(addr_size size) {
    // flags - memory is private copy-on-write and is not backed by a file, i.e. anonymous
    i32 flags = ( MAP_PRIVATE | MAP_ANONYMOUS );
    // protection - memory is mapped for reading and for writing.
    i32 prot = ( PROT_READ | PROT_WRITE );

    void* addr = mmap(nullptr, size, prot, flags, 0, 0);
    if (addr == MAP_FAILED || addr == nullptr) {
        return unexpected(u64(errno));
    }
    return addr;
}

expected<plt_err_code> os_dealloc_pages(void *addr, addr_size size) {
    if (addr == nullptr) {
        return unexpected(OS_DEALLOC_NULL_ADDR_ERR);
    }
    i32 ret = munmap(addr, size);
    if (ret < 0) {
        // munmap returns -1 on error and sets errno
        return unexpected(u64(errno));
    }
    return {};
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    struct timeval now;
    i64 ret = gettimeofday(&now, 0);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    u64 timeInMs = ((u64) now.tv_sec) * 1000 + ((u64) now.tv_usec) / 1000;
    return timeInMs;
}

expected<plt_err_code> os_thread_sleep(u64 ms) {
    struct timespec rem;
    struct timespec req = {
        u32((ms / 1000)),/* secs (Must be Non-Negative) */
        u32((ms % 1000) * 1000000) /* nano (Must be in range of 0 to 999999999) */
    };
    i32 ret = nanosleep(&req , &rem);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

namespace {
static addr_size g_default_block_size = 4 * core::KILOBYTE;
} // namespace

void os_set_default_block_size(addr_size size) { g_default_block_size = size; }
addr_size os_get_default_block_size() { return g_default_block_size; }

file_access_group default_file_access_group() {
    file_access_group ret = { file_access::FA_ReadWrite, file_access::FA_ReadWrite, file_access::FA_Read };
    return ret;
}

file_flags default_file_flags() {
    file_flags ret = file_flags(FF_ReadWrite);
    return ret;
}

file_params default_file_params() {
    file_params ret = {};
    ret.access = default_file_access_group();
    ret.flags = default_file_flags();
    ret.osSpecificFlags = 0;
    return ret;
}

namespace {

constexpr mode_t file_access_to_osmode(file_access_group access) {
    const file_access user = access.user;
    const file_access group = access.group;
    const file_access other = access.other;
    mode_t osmode = 0;

    if (user & file_access::FA_Read)     osmode |= S_IRUSR;
    if (user & file_access::FA_Write)    osmode |= S_IWUSR;
    if (user & file_access::FA_Execute)  osmode |= S_IXUSR;
    if (group & file_access::FA_Read)    osmode |= S_IRGRP;
    if (group & file_access::FA_Write)   osmode |= S_IWGRP;
    if (group & file_access::FA_Execute) osmode |= S_IXGRP;
    if (other & file_access::FA_Read)    osmode |= S_IROTH;
    if (other & file_access::FA_Write)   osmode |= S_IWOTH;
    if (other & file_access::FA_Execute) osmode |= S_IXOTH;

    return osmode;
}

constexpr i32 file_flags_to_osflags(file_flags flags) {
    i32 osflags = 0;

    if (flags & file_flags::FF_ReadWrite)      osflags |= O_RDWR;
    else if (flags & file_flags::FF_ReadOnly)  osflags |= O_RDONLY;
    else if (flags & file_flags::FF_WriteOnly) osflags |= O_WRONLY;

    if (flags & file_flags::FF_Append)     osflags |= O_APPEND;
    if (flags & file_flags::FF_Create)     osflags |= O_CREAT;
    if (flags & file_flags::FF_Trunc)      osflags |= O_TRUNC;
    if (flags & file_flags::FF_Large)      osflags |= O_LARGEFILE;
    if (flags & file_flags::FF_Tmp)        osflags |= O_TMPFILE;

    return osflags;
}

} // namespace

expected<file_desc, plt_err_code> os_open(const char* path, const file_params& params) {
    mode_t osmode = file_access_to_osmode(params.access);
    i32 flags = file_flags_to_osflags(params.flags);
    i32 ret = open(path, flags | params.osSpecificFlags, osmode);
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    file_desc desc = file_desc{ reinterpret_cast<void*>((u64)(ret)) };
    return desc;
}

expected<file_desc, plt_err_code> os_opendir(const char* path) {
    file_access_group access = default_file_access_group();
    file_flags flags = FF_None;
    i32 osflags = O_RDONLY | O_DIRECTORY | O_CLOEXEC;
    return os_open(path, { access, flags, osflags});
}

expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead) {
    bytesRead = read(i32(fd.to_u64()), buf, size);
    if (bytesRead < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten) {
    bytesWritten = write(i32(fd.to_u64()), buf, size);
    if (bytesWritten < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_close(file_desc fd) {
    i32 ret = close(i32(fd.to_u64()));
    if (ret < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_rmfile(const char* path) {
    i32 res = unlink(path);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_mkdir(const char* path, const file_access_group& access) {
    mode_t mode = file_access_to_osmode(access);
    i32 res = mkdir(path, mode);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<plt_err_code> os_rmdir(const char* path) {
    i32 res = rmdir(path);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return {};
}

expected<bool, plt_err_code> os_exists(const char* path) {
    i32 res = access(path, F_OK);
    if (res < 0) {
        if (errno == ENOENT) {
            // File was simply not found.
            return false;
        }
        // Some other unexpected error occurred:
        return unexpected(u64(errno));
    }
    return true;
}

namespace {

file_stat to_file_stat(struct stat& st) {
    file_stat fs;
    fs.mtime = (u64) st.st_mtime;
    fs.size = (addr_size) st.st_size;
    fs.blksize = (addr_size) st.st_blksize;
    fs.access.user = file_access::FA_None;
    fs.access.group = file_access::FA_None;
    fs.access.other = file_access::FA_None;

    switch (st.st_mode & S_IFMT) {
        case S_IFREG:  fs.type = file_type::Regular;        break;
        case S_IFDIR:  fs.type = file_type::Directory;      break;
        case S_IFCHR:  fs.type = file_type::UnixCharDevice; break;
        case S_IFBLK:  fs.type = file_type::Device;         break;
        case S_IFIFO:  fs.type = file_type::NamedPipe;      break;
        case S_IFLNK:  fs.type = file_type::Symlink;        break;
        case S_IFSOCK: fs.type = file_type::Socket;         break;

        default: fs.type = file_type::Unknown;
    }

    if (st.st_mode & S_IRUSR) fs.access.user = file_access(fs.access.user | u32(file_access::FA_Read));
    if (st.st_mode & S_IWUSR) fs.access.user = file_access(fs.access.user | u32(file_access::FA_Write));
    if (st.st_mode & S_IXUSR) fs.access.user = file_access(fs.access.user | u32(file_access::FA_Execute));
    if (st.st_mode & S_IRGRP) fs.access.group = file_access(fs.access.group | u32(file_access::FA_Read));
    if (st.st_mode & S_IWGRP) fs.access.group = file_access(fs.access.group | u32(file_access::FA_Write));
    if (st.st_mode & S_IXGRP) fs.access.group = file_access(fs.access.group | u32(file_access::FA_Execute));
    if (st.st_mode & S_IROTH) fs.access.other = file_access(fs.access.other | u32(file_access::FA_Read));
    if (st.st_mode & S_IWOTH) fs.access.other = file_access(fs.access.other | u32(file_access::FA_Write));
    if (st.st_mode & S_IXOTH) fs.access.other = file_access(fs.access.other | u32(file_access::FA_Execute));

    return fs;
}

} // namespace

expected<file_stat, plt_err_code> os_stat(const char* path) {
    struct stat st;
    i32 res = stat(path, &st);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    file_stat fs = to_file_stat(st);
    return fs;
}

expected<file_stat, plt_err_code> os_fstat(file_desc fd) {
    struct stat st;
    i32 res = fstat(i32(fd.to_u64()), &st);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    file_stat fs = to_file_stat(st);
    return fs;
}

expected<addr_off, plt_err_code> os_seek(file_desc fd, addr_off offset, seek_origin origin) {
    i32 whence = 0;
    switch (origin) {
        case seek_origin::Begin:   whence = SEEK_SET; break;
        case seek_origin::Current: whence = SEEK_CUR; break;
        case seek_origin::End:     whence = SEEK_END; break;
    }
    i64 res = lseek(i32(fd.to_u64()), offset, whence);
    if (res < 0) {
        return unexpected(u64(errno));
    }
    return addr_off(res);
}

namespace {
AtExitCb g_atExit;
} // namespace

void os_exit(i32 exitCode) {
    if (g_atExit) g_atExit(exitCode);
    _exit(exitCode);
}

void at_exit(AtExitCb atExit) {
    g_atExit = atExit;
}

const char* os_get_err_cptr(plt_err_code err) {
    return std::strerror(i32(err));
}

} // namespace core
