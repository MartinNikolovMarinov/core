#include <plt/core_fs.h>

namespace core {

expected<PltErrCode> fileReadEntire(const char* path, ArrList<u8>& out) {
    using value_type = typename core::ArrList<u8>::value_type;

    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Read);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = std::move(res.value());
    }
    defer {
        fileClose(file);
    };

    addr_size size = 0;
    {
        auto res = fileSize(file);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        size = res.value();
    }

    if (size == 0) return {};

    if (out.len() < size) {
        // Deliberately avoiding zeroing out memory here!
        auto data = reinterpret_cast<value_type*>(getAllocator(DEFAULT_ALLOCATOR_ID).alloc(size, sizeof(u8)));
        out.reset(&data, size, size);
    }

    addr_size readbytes = 0;
    while (readbytes < size) {
        auto res = fileRead(file, out.data() + readbytes, size - readbytes);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        readbytes += res.value();
    }

    return {};
}

core::expected<PltErrCode> fileWriteEntire(const char* path, const u8* data, addr_size size) {
    if (size == 0) return {};

    FileDesc file;
    {
        auto res = fileOpen(path, OpenMode::Write | OpenMode::Create | OpenMode::Truncate);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        file = std::move(res.value());
    }
    defer {
        fileClose(file);
    };

    addr_size written = 0;
    while (written < size) {
        auto res = fileWrite(file, data + written, size - written);
        if (res.hasErr()) {
            return core::unexpected(res.err());
        }
        written += res.value();
    }

    return {};
}

core::expected<PltErrCode> fileWriteEntire(const char* path, const core::ArrList<u8>& in) {
    return fileWriteEntire(path, in.data(), in.len());
}

core::expected<PltErrCode> dirDeleteRec(const char* path) {
    using core::StrBuilder;
    using DirectoryNames = core::ArrList<core::StrBuilder<>>;

    StrBuilder fileNameTmpSb;
    DirectoryNames dirNames;
    dirNames.push(StrBuilder(core::sv(path)));
    addr_size workIdx = 0;

    struct Closure {
        DirectoryNames& dirNames;
        StrBuilder<>& fileNameBufferSb;
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
                StrBuilder newDirName = curr.copy();
                newDirName.append(PATH_SEPARATOR);
                newDirName.append(core::sv(entry.name));
                _dirNames.push(std::move(newDirName));
            }
            else {
                // Found a file, delete it.
                _fileNameBufferSb.clear();
                _fileNameBufferSb.append(curr.view());
                _fileNameBufferSb.append(PATH_SEPARATOR);
                _fileNameBufferSb.append(core::sv(entry.name));
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

expected<bool, PltErrCode> dirIsEmpty(const char* path) {
    struct Closure {
        bool isEmpty;
    };

    auto emtpyDirWalk = [](const DirEntry&, addr_size, void* closure) {
        Closure* c = reinterpret_cast<Closure*>(closure);
        c->isEmpty = false;
        return false;
    };

    Closure c = {true};
    auto res = dirWalk(path, emtpyDirWalk, reinterpret_cast<void*>(&c));

    if (res.hasErr()) {
        return core::unexpected(res.err());
    }

    bool ret = c.isEmpty;
    return ret;
}

} // namespace core
