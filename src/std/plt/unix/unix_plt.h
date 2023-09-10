#pragma once

#include <std/plt.h>

#include <arr.h>
#include <char_ptr.h>
#include <utils.h>

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
expected<plt_err_code> os_dir_walk(file_desc fd, const TWalkerFn& cb) {
    const addr_off blockSize = (addr_off) os_get_default_block_size();
    off_t basep = 0;

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
}

} // namespace core
