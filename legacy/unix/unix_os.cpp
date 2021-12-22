#include "../core.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h> // NOTE: POSIX requires that errno is threadsafe!
#include <unistd.h>
#include <sys/mman.h>

namespace core
{

void OsExit(i32 _code) { exit(_code); }

intern_fn i32 ConvertOpenFlagsToSystemFlag(OpenFlag _oflag)
{
    i32 sysFlags = 0;

    if (((i32)_oflag & (i32)OpenFlag::READ_ONLY) > 0)
        sysFlags = O_RDONLY;
    if (((i32)_oflag & (i32)OpenFlag::WRITE_ONLY) > 0)
        sysFlags = O_WRONLY;
    if (((i32)_oflag & (i32)OpenFlag::READ_WRITE) > 0)
        sysFlags = O_RDWR;

    if (((i32)_oflag & (i32)OpenFlag::APPEND) > 0)
        sysFlags |= O_APPEND;
    if (((i32)_oflag & (i32)OpenFlag::CREATE) > 0)
        sysFlags |= O_CREAT;
    if (((i32)_oflag & (i32)OpenFlag::TRUNC) > 0)
        sysFlags |= O_TRUNC;

    return sysFlags;
}

Optional<OsFile, OSErrCode> OsOpen(constptr char* _path, OpenFlag _oflag) { return OsOpen(_path, _oflag, 0777); }
Optional<OsFile, OSErrCode> OsOpen(constptr char* _path, OpenFlag _oflag, i32 _osSpecificPerms)
{
    // NOTE: _osSpecificPerms on UNIX:
    // 0000    no permissions
    // 0700    read, write, & execute only for owner
    // 0770    read, write, & execute for owner and group
    // 0777    read, write, & execute for owner, group and others
    // 0111    execute
    // 0222    write
    // 0333    write & execute
    // 0444    read
    // 0555    read & execute
    // 0666    read & write
    // 0740    owner can read, write, & execute; group can only read; others have no permissions

    i32 sysFlags = ConvertOpenFlagsToSystemFlag(_oflag);
    i32 fd = open(_path, sysFlags, _osSpecificPerms);
    OsFile ret = OsFile(fd);
    if (fd <= 0) {
        return Optional<OsFile, OSErrCode>(ret, errno);
    }
    return Optional<OsFile, OSErrCode>(ret, null);
}

Optional<signed_ptr_size, OSErrCode> OsWrite(OsFile _file, const void* _buf, ptr_size _n)
{
    signed_ptr_size ret = write(_file.ToFd(), _buf, _n);
    if (ret < 0) {
        return Optional<signed_ptr_size, OSErrCode>(ret, errno);
    }
    return Optional<signed_ptr_size, OSErrCode>(ret, null);
}

Optional<signed_ptr_size, OSErrCode> OsRead(OsFile _file, modptr void* _buf, ptr_size _n)
{
    signed_ptr_size ret = read(_file.ToFd(), _buf, _n);
    if (ret < 0) {
        return Optional<signed_ptr_size, OSErrCode>(ret, errno);
    }
    return Optional<signed_ptr_size, OSErrCode>(ret, null);
}

Optional<i32, OSErrCode> OsClose(OsFile _file)
{
    i32 ret = close(_file.ToFd());
    if (ret < 0) {
        return Optional<i32, OSErrCode>(ret, errno);
    }
    return Optional<i32, OSErrCode>(ret, null);
}

Optional<i32, OSErrCode> OsRemove(constptr char *_path)
{
    i32 ret = remove(_path);
    if (ret < 0) {
        return Optional<i32, OSErrCode>(ret, errno);
    }
    return Optional<i32, OSErrCode>(ret, null);
}

Optional<void*, OSErrCode> OsAllocPages(ptr_size _size)
{
    // flags - memory is private copy-on-write and is not backed by a file, i.e. anonymous
    i32 flags = ( MAP_PRIVATE | MAP_ANONYMOUS );
    // port - memory is mapped for reading and for writing.
    i32 prot = ( PROT_READ | PROT_WRITE );

    void* addr = mmap(null, _size, prot, flags, null, 0);
    if (addr != MAP_FAILED && addr != null) {
        return Optional<void*, OSErrCode>(addr, null);
    }

    return Optional<void*, OSErrCode>(addr, null);
}

Optional<bool8, OSErrCode> OsDeallocPages(void *_addr, ptr_size _size)
{
    i32 ret = munmap(_addr, _size);
    if (ret < 0) {
        return Optional<bool8, OSErrCode>(true, errno);
    }
    return Optional<bool8, OSErrCode>(false, null);
}

Optional<OsMemInfo, OSErrCode> OsGetUsedMemoryInfo()
{
    Panic("TODO: not implemented yet");
    OsMemInfo ret = {};
    return Optional<OsMemInfo, OSErrCode>(ret, null);
}

} // namespace core