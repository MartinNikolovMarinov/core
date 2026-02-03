#pragma once

#include "core_arr.h"
#include "core_exec_ctx.h"
#include "core_expected.h"
#include "core_str_builder.h"
#include "core_types.h"

#include "plt/core_fs.h"
#include "plt/core_path.h"
#include "plt/core_plt_error.h"

namespace core {

using namespace coretypes;

template <core::AllocatorId TAllocatorId>
core::expected<PltErrCode> dirDeleteRec(const char* path) {
    using core::StrBuilder;
    using DirectoryNames = core::ArrList<core::StrBuilder<TAllocatorId>>;

    StrBuilder<TAllocatorId> fileNameTmpSb;
    DirectoryNames dirNames;
    dirNames.push(StrBuilder<TAllocatorId>(core::sv(path)));
    addr_size workIdx = 0;

    struct Closure {
        DirectoryNames& dirNames;
        StrBuilder<TAllocatorId>& fileNameBufferSb;
        PltErrCode& errCode;
        addr_size idx;
    };

    // Delete all files in the directory tree.
    while (workIdx < dirNames.len()) {
        PltErrCode fileDelErrCode = ERR_PLT_NONE;

        DirWalkCallback deleteWalk = [](const DirEntry& entry, addr_size, void* userData) -> bool {
            Closure& d = *static_cast<Closure*>(userData);
            auto& _dirNames = d.dirNames;
            auto& _fileNameBufferSb = d.fileNameBufferSb;
            auto& _workIdx = d.idx;
            auto& _errCode = d.errCode;

            const auto& curr = _dirNames[_workIdx];

            if (entry.type == FileType::Directory) {
                // Found a directory, add it to the list of directories to delete later.
                StrBuilder<TAllocatorId> newDirName = curr.copy();
                newDirName.append(PATH_SEPARATOR);
                newDirName.append(core::sv(entry.name));
                // newDirName.append('\0'); // TODO: [BUG] Investigate why this creates a bug.
                _dirNames.push(std::move(newDirName));
            }
            else {
                // Found a file, delete it.
                _fileNameBufferSb.clear();
                _fileNameBufferSb.append(curr.view());
                _fileNameBufferSb.append(PATH_SEPARATOR);
                _fileNameBufferSb.append(core::sv(entry.name));
                _fileNameBufferSb.append('\0');
                const char* fullFilePath = _fileNameBufferSb.view().data();
                if (auto dres = fileDelete(fullFilePath); dres.hasErr()) {
                    _errCode = std::move(dres.err());
                    return false;
                }
            }

            return true;
        };

        Closure closure = { dirNames, fileNameTmpSb, fileDelErrCode, workIdx };
        auto res = dirWalk(dirNames[workIdx].view().data(), deleteWalk, reinterpret_cast<void*>(&closure));

        if (res.hasErr()) {
            return core::unexpected(res.err());
        }

        if (fileDelErrCode != ERR_PLT_NONE) {
            return core::unexpected(fileDelErrCode);
        }

        workIdx++;
    }

    // All directories should be empty by now, and thus deletable.
    for (addr_size i = dirNames.len(); i > 0; i--) {
        const char* dirPath = dirNames[i - 1].view().data();
        if (auto dres = dirDelete(dirPath); dres.hasErr()) {
            return core::unexpected(dres.err());
        }
    }

    return {};
}

} // namespace core
