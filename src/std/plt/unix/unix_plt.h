#pragma once

#include <std/plt.h>

#include <arr.h>
#include <char_ptr.h>
#include <utils.h>

#include <stdio.h>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

namespace core {

namespace detail {

inline dir_entry to_dir_entry(const dirent& d) {
    dir_entry de = {};
    de.name = d.d_name; // d.d_name should be null terminated according to documentation.
    switch (d.d_type) {
        case DT_REG:
            de.type = file_type::Regular;
            break;
        case DT_DIR:
            de.type = file_type::Directory;
            break;
        case DT_FIFO:
            de.type = file_type::NamedPipe;
            break;
        case DT_SOCK:
            de.type = file_type::Socket;
            break;
        case DT_CHR:
            de.type = file_type::UnixCharDevice;
            break;
        case DT_BLK:
            de.type = file_type::Device;
            break;
        case DT_LNK:
            de.type = file_type::Symlink;
            break;
        default:
            de.type = file_type::Unknown;
    }

    return de;
}

};

template <typename TWalkerFn>
expected<plt_err_code> os_dir_walk(const char* path, const TWalkerFn& cb) {

    // IMPORTANT: 
    //
    // Big thanks to the MacOS developers.
    //
    // readdir_r and getdirentries are both depricated on Mac so do NOT use them! 
    // Very unfortunelty the std directory api forces dynamic memory allocation and it does it under the hood without giving any control over how it happens.
    // I hope some std implementations don't allocate memory, but musl, for example, does.
    //
    // Another very annoying thing:
    //
    // If this functions needs to be extended to use file_desc it needs to copy the underlining fd! 
    // The DIR* created from fdreaddir uses the same fd that it is passed. That is fine, but it also allcoates memory for DIR* and that memory is freed by 
    // closedir. So I can't just use close on the file descriptor. After I use opendir I have to use closedir, which complicates the API. It can, possibly
    // be implemented with some more involved abstraction layer, but that defeates the pourpuse of the plt layer, which I want to remain as tin as possible!
    //
    // That is the reason why this function takes path instead of file_desc.
    //
    // In conclusion I am stuck with this implementation whether I like it or not.

    DIR* dir = opendir(path);
    if (!dir) {
        return unexpected(plt_err_code(errno));
    }

    errno = 0;

    dirent* entry;
    while (true) {
        entry = readdir(dir);
        if (entry == nullptr) break; // reached the last directory entry

        bool shouldSkip = core::cptr_eq(entry->d_name, ".", 1) || core::cptr_eq(entry->d_name, "..", 2);
        if (shouldSkip) continue;

        dir_entry de = detail::to_dir_entry(*entry);
        if (!cb(de)) break;
    }

    if (errno != 0) {
        // readdir sets errno to non-zero on error and there is no other way to tell if an error occured.
        return unexpected(plt_err_code(errno));
    }

    if (closedir(dir) < 0) {
        return unexpected(plt_err_code(errno));
    }

    return {};

#if 0
    // TODO: I like this implementation better, but it is no longer possible on MAC, because they depricated getdirentries... I kinda don't want to delete it just yet.

    const addr_off blockSize = (addr_off) os_get_default_block_size();

    long basep = 0;

    while (true) {

        // Make the system call to get the entries.

        char buff[blockSize];
        addr_off currReadBytes = (addr_off) getdirentries(i32(fd.to_u64()), buff, blockSize, &basep);
        if (currReadBytes < 0) {
            return unexpected(u64(errno));
        }
        if (currReadBytes == 0) {
            // End of directory according to documentation.
            break;
        }

        // Parse the buffer which contains directory entries:

        addr_off recByteOffset = 0;
        do {
            dirent* d = reinterpret_cast<dirent*>(buff + recByteOffset);
            recByteOffset += d->d_reclen;
            basep = d->d_off;
            dir_entry de = detail::to_dir_entry(*d);
            bool shouldSkip = core::cptr_eq(de.name, ".", 1) || core::cptr_eq(de.name, "..", 2);
            if (!shouldSkip) {
                cb(de); // Let's hope this gets inlined. Fingers corssed...
            }
        } while (recByteOffset < currReadBytes);

        if (currReadBytes < blockSize) {
            // If the actual bytes read are less than the block size, then the end of the directory has been reached and
            // there is no need to make another system call.
            break;
        }
    }

    return {};
#endif
}

} // namespace core
