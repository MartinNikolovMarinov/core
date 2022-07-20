#include "../core.h"

#include <windows.h>
#include <psapi.h>
#include <errhandlingapi.h> // NOTE: GetLastError() is thread safe!

namespace core
{

void OsExit(i32 _code)
{
    exit(_code);
}

Optional<OsFile, OSErrCode> OsOpen(constptr char* _path, OpenFlag _oflag)
{
    return OsOpen(_path, _oflag, 0);
}
Optional<OsFile, OSErrCode> OsOpen(constptr char* _path, OpenFlag _oflag, i32 _osSpecificPerms)
{
    // FIXME: implement _osSpecificPerms
    // Should probably use the ACL (Access Control Lists) or the older DOS version, not sure..

    i32 desiredAccess = 0;
    i32 creationDisposition = 0;

    if (((i32)_oflag & (i32)OpenFlag::CREATE) > 0) {
        creationDisposition = CREATE_ALWAYS;
        desiredAccess =  GENERIC_READ | GENERIC_WRITE;
    } else {
        creationDisposition = OPEN_EXISTING;
        if (((i32)_oflag & (i32)OpenFlag::READ_ONLY) > 0) {
            desiredAccess |= GENERIC_READ;
        }
        if (((i32)_oflag & (i32)OpenFlag::WRITE_ONLY) > 0) {
            desiredAccess |= GENERIC_WRITE;
        }
        if (((i32)_oflag & (i32)OpenFlag::READ_WRITE) > 0) {
            desiredAccess |= GENERIC_READ | GENERIC_WRITE;
        }
    }

    if (((i32)_oflag & (i32)OpenFlag::APPEND) > 0) {
        desiredAccess |= FILE_APPEND_DATA;
    }
    if (((i32)_oflag & (i32)OpenFlag::TRUNC) > 0) {
        Panic("TODO: TRUNC is not implemented yet.");
    }


    // Other processes can open the file for reading, but can't open for writing!
    // TODO: might want to add an option for FILE_SHARE_READ, FILE_SHARE_WRITE and FILE_SHARE_DELETE
    // Not sure how to do it for the unix part, or if is at all needed.
    i32 shareMode = FILE_SHARE_READ;

    HANDLE handle = CreateFileA(_path, desiredAccess, shareMode, null, creationDisposition, FILE_ATTRIBUTE_NORMAL, null);
    OsFile ret = OsFile();
    ret.file = (void *)handle;
    if (handle == INVALID_HANDLE_VALUE) {
        return Optional<OsFile, OSErrCode>(ret, (OSErrCode)GetLastError());
    }

    return Optional<OsFile, OSErrCode>(ret, 0);
}

Optional<signed_ptr_size, OSErrCode> OsWrite(OsFile _file, const void* _buf, ptr_size _n)
{
    // FileWrite
    Panic("TODO: not implemented yet!");
    return Optional<signed_ptr_size, OSErrCode>(0, 0);
}

Optional<signed_ptr_size, OSErrCode> OsRead(OsFile _file, modptr void* _buf, ptr_size _n)
{
    // FileRead
    Panic("TODO: not implemented yet!");
    return Optional<signed_ptr_size, OSErrCode>(0, 0);
}

Optional<i32, OSErrCode> OsClose(OsFile _file)
{
    // FileClose
    Panic("TODO: not implemented yet!");
    return Optional<i32, OSErrCode>(0, 0);
}

Optional<i32, OSErrCode> OsRemove(constptr char *_path)
{
    Panic("TODO: not implemented yet!");
    return Optional<i32, OSErrCode>(0, 0);
}

Optional<void*, OSErrCode> OsAllocPages(ptr_size _size)
{
    void* addr = VirtualAlloc(null, _size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (addr == null) {
        return Optional<void*, OSErrCode>(null, (OSErrCode)GetLastError());
    }
    return Optional<void*, OSErrCode>(addr, null);
}

Optional<bool8, OSErrCode> OsDeallocPages(void *_addr, ptr_size _size)
{
    // size is ignred for the win64 api. VirtualFree deallocates all memory at the address.
    BOOL ret = VirtualFree(_addr, 0, MEM_RELEASE);
    if (ret == 0) {
        return Optional<bool8, OSErrCode>(false, (OSErrCode)GetLastError());
    }
    return Optional<bool8, OSErrCode>(true, null);
}

Optional<OsMemInfo, OSErrCode> OsGetUsedMemoryInfo()
{
   /**
    * NOTE: other info ion the PROCESS_MEMORY_COUNTERS struct:
    *
    * PrintF( "PageFaultCount: %llu\n", (u64)pmc.PageFaultCount );
    * PrintF( "PeakWorkingSetSize: %llu\n", pmc.PeakWorkingSetSize );
    * PrintF( "WorkingSetSize: %llu\n", pmc.WorkingSetSize );
    * PrintF( "QuotaPeakPagedPoolUsage: %llu\n", pmc.QuotaPeakPagedPoolUsage );
    * PrintF( "QuotaPagedPoolUsage: %llu\n", pmc.QuotaPagedPoolUsage );
    * PrintF( "QuotaPeakNonPagedPoolUsage: %llu\n", pmc.QuotaPeakNonPagedPoolUsage );
    * PrintF( "QuotaNonPagedPoolUsage: %llu\n", pmc.QuotaNonPagedPoolUsage );
    * PrintF( "PagefileUsage: %llu\n", pmc.PagefileUsage );
    * PrintF( "PeakPagefileUsage: %llu\n", pmc.PeakPagefileUsage );
    */
    PROCESS_MEMORY_COUNTERS pmc;
    OsMemInfo ret = {};

    BOOL getMemInfoRet = GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    if (getMemInfoRet != TRUE) {
        return Optional<OsMemInfo, OSErrCode>(ret, (OSErrCode)GetLastError());
    }

    ret.UsedVirtualMem = pmc.PagefileUsage;
    return Optional<OsMemInfo, OSErrCode>(ret, null);
}

} // namespace core
