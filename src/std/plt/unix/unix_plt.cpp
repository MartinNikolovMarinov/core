#include <std/plt/unix/unix_plt.h>

namespace core {

file_desc::file_desc()             : native((void*)(-1)) {}
file_desc::file_desc(void* native) : native(native) {}

u64 file_desc::to_u64() { return (u64)native; }

expected<void*, plt_err_code> os_alloc_pages(addr_size size) {
    // flags - memory is private copy-on-write and is not backed by a file, i.e. anonymous
    i32 flags = ( MAP_PRIVATE | MAP_ANONYMOUS );
    // protection - memory is mapped for reading and for writing.
    i32 prot = ( PROT_READ | PROT_WRITE );

    void* addr = mmap(nullptr, size, prot, flags, 0, 0);
    if (addr == MAP_FAILED || addr == nullptr) {
        return unexpected(plt_err_code(errno));
    }
    return addr;
}

expected<plt_err_code> os_dealloc_pages(void *addr, addr_size size) {
    if (addr == nullptr) {
        return unexpected(ERR_OS_DEALLOC_NULL_ADDR_ERR);
    }
    i32 ret = munmap(addr, size);
    if (ret < 0) {
        // munmap returns -1 on error and sets errno
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<u64, plt_err_code> os_unix_time_stamp_in_ms() {
    struct timeval now;
    i64 ret = gettimeofday(&now, 0);
    if (ret < 0) {
        return unexpected(plt_err_code(errno));
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
        return unexpected(plt_err_code(errno));
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

file_access_group default_dir_access_group() {
    file_access_group ret = { file_access::FA_All, file_access::FA_All, file_access::FA_Read };
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

    return osflags;
}

} // namespace

expected<file_desc, plt_err_code> os_open(const char* path, const file_params& params) {
    mode_t osmode = file_access_to_osmode(params.access);
    i32 flags = file_flags_to_osflags(params.flags);
    i32 ret = open(path, flags | params.osSpecificFlags, osmode);
    if (ret < 0) {
        return unexpected(plt_err_code(errno));
    }
    file_desc desc = file_desc{ reinterpret_cast<void*>((u64)(ret)) };
    return desc;
}

expected<file_desc, plt_err_code> os_create(const char* path, const file_access_group& access) {
    file_params params;
    params.access = access;
    params.flags = file_flags(FF_Create | FF_WriteOnly | FF_Trunc);
    params.osSpecificFlags = 0;
    return os_open(path, params);
}

expected<file_desc, plt_err_code> os_opendir(const char* path) {
    file_access_group access = default_file_access_group();
    file_flags flags = FF_None;
    i32 osflags = O_RDONLY | O_DIRECTORY | O_CLOEXEC;
    return os_open(path, { access, flags, osflags});
}

expected<file_desc, plt_err_code> os_opencwd() {
    return os_opendir(".");
}

expected<plt_err_code> os_read(file_desc fd, void* buf, u64 size, i64& bytesRead) {
    bytesRead = read(i32(fd.to_u64()), buf, size);
    if (bytesRead < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_write(file_desc fd, const void* buf, u64 size, i64& bytesWritten) {
    bytesWritten = write(i32(fd.to_u64()), buf, size);
    if (bytesWritten < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_close(file_desc fd) {
    i32 ret = close(i32(fd.to_u64()));
    if (ret < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_truncate(file_desc fd, addr_off size) {
    i32 res = ftruncate(i32(fd.to_u64()), size);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_rmfile(const char* path) {
    i32 res = unlink(path);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_mkdir(const char* path, const file_access_group& access) {
    mode_t mode = file_access_to_osmode(access);
    i32 res = mkdir(path, mode);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_rmdir(const char* path) {
    i32 res = rmdir(path);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_chdir(const char* path) {
    i32 res = chdir(path);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_fchdir(file_desc fd) {
    i32 res = fchdir(i32(fd.to_u64()));
    if (res < 0) {
        return unexpected(plt_err_code(errno));
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
        return unexpected(plt_err_code(errno));
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
        return unexpected(plt_err_code(errno));
    }
    file_stat fs = to_file_stat(st);
    return fs;
}

expected<file_stat, plt_err_code> os_fstat(file_desc fd) {
    struct stat st;
    i32 res = fstat(i32(fd.to_u64()), &st);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
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
        return unexpected(plt_err_code(errno));
    }
    return addr_off(res);
}

expected<plt_err_code> os_flush(file_desc fd) {
#if defined(OS_MAC) && OS_MAC == 1
    i32 res = fcntl(i32(fd.to_u64()), F_FULLFSYNC);
#else
    i32 res = fsync(i32(fd.to_u64()));
#endif

    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_rename(const char* oldPath, const char* newPath) {
    i32 res = rename(oldPath, newPath);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

expected<plt_err_code> os_access(file_desc fd, const file_access_group& access) {
    mode_t mode = file_access_to_osmode(access);
    i32 res = fchmod(i32(fd.to_u64()), mode);
    if (res < 0) {
        return unexpected(plt_err_code(errno));
    }
    return {};
}

namespace {

i32 to_native_signal(core_signal sig) {
    switch (sig) {
        case core_signal::CORE_SIGABRT: return SIGABRT;
        case core_signal::CORE_SIGFPE:  return SIGFPE;
        case core_signal::CORE_SIGILL:  return SIGILL;
        case core_signal::CORE_SIGINT:  return SIGINT;
        case core_signal::CORE_SIGSEGV: return SIGSEGV;
        case core_signal::CORE_SIGTERM: return SIGTERM;
        case core_signal::CORE_SIGKILL: return SIGKILL;
        case core_signal::CORE_SIGSTOP: return SIGSTOP;

        case core_signal::SENTINEL: return -1;
    }

    return -1;
}

core_signal from_native_signal(i32 sig) {
    switch (sig) {
        case SIGABRT: return core_signal::CORE_SIGABRT;
        case SIGFPE:  return core_signal::CORE_SIGFPE;
        case SIGILL:  return core_signal::CORE_SIGILL;
        case SIGINT:  return core_signal::CORE_SIGINT;
        case SIGSEGV: return core_signal::CORE_SIGSEGV;
        case SIGTERM: return core_signal::CORE_SIGTERM;
        case SIGKILL: return core_signal::CORE_SIGKILL;
        case SIGSTOP: return core_signal::CORE_SIGSTOP;

        default: return core_signal::SENTINEL;
    }

    return core_signal::SENTINEL;
}

} // namespace

expected<plt_err_code> os_send_signal(core_signal sig, thread_id threadId) {
    i32 nsig = to_native_signal(sig);
    if (nsig < 0) {
        return unexpected(ERR_OS_UNSUPPORTED_SIGNAL);
    }

    i32 res;
    if (threadId == 0) {
        res = raise(nsig);
    }
    else {
        // I can technically use the system thread id and the kill system call, but that will be inconsistent with the
        // threading library.
        res = pthread_kill(pthread_t(threadId), nsig);
    }

    if (res < 0) {
        return unexpected(plt_err_code(res));
    }

    return {};
}

expected<signal_handler, plt_err_code> os_register_signal_handler(core_signal sig, signal_handler handler) {
    i32 nsig = to_native_signal(sig);
    if (nsig < 0) {
        return unexpected(ERR_OS_UNSUPPORTED_SIGNAL);
    }

    signal_handler prevHandler = signal(nsig, handler);
    if (prevHandler == SIG_ERR) {
        return unexpected(plt_err_code(errno));
    }

    return prevHandler;
}

const char* os_get_err_cptr(plt_err_code err) {
    if (err < detail::ERR_START_OF_CUSTOM_ERRORS) {
        return std::strerror(i32(err));
    }

    auto res = custom_plt_err_code_to_cptr(err);
    return res;
}

} // namespace core
