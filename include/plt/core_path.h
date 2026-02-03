#pragma once

#include <core_API.h>
#include <core_algorithms.h>
#include <core_assert.h>
#include <core_cstr.h>
#include <core_mem.h>
#include <core_str_view.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

static constexpr char PATH_SEPARATOR = '/';

template <typename T>
concept Path =
    requires {
        requires std::is_same_v<decltype(&T::fullPath), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::filePart), const char* (T::*)() const>;
        requires std::is_same_v<decltype(&T::extPart), const char* (T::*)() const>;
    };

struct CORE_API_EXPORT ImmutablePath {
    const char* ptr;
    const addr_size len;

    constexpr ImmutablePath(const char* p, addr_size l) : ptr(p), len(l) {}
    constexpr ImmutablePath(const char* p) : ptr(p), len(core::cstrLen(p)) {}

    constexpr const char* filePart() const {
        if (len == 0) {
            return ptr;
        }

        const char* p = ptr + len;
        while (p != ptr) {
            if (*(p - 1) == PATH_SEPARATOR) {
                return p;
            }
            --p;
        }

        return ptr;
    }

    constexpr const char* extPart() const {
        const char* start = filePart();
        const char* p = ptr + len;
        while (p != start) {
            if (*(p - 1) == '.') {
                return p - 1;
            }
            --p;
        }

        return ptr + len;
    }

    constexpr const char* fullPath() const { return ptr; }
};

static_assert(Path<ImmutablePath>, "ImmutablePath does not satisfy Path concept");

template <i32 TBufferSize>
struct StaticPathBuilder {
    char buff[TBufferSize] = {};
    i32 len = 0;

    //==================================================================================================================
    // Get Parts
    //==================================================================================================================

    constexpr const char* fullPath() const {
        return buff;
    }
    constexpr core::StrView fullPathSv() const {
        auto ret = core::sv(buff, addr_size(len));
        return ret;
    }

    constexpr const char* filePart() const {
        return filePartSv().data();
    }
    constexpr core::StrView filePartSv() const {
        addr_off fOff = fileOff();
        if (buff[fOff] == core::PATH_SEPARATOR) fOff++;
        auto ret = core::sv(buff + fOff, addr_size(addr_off(len) - fOff));
        return ret;
    }

    constexpr const char* extPart() const {
        return extPartSv().data();
    }
    constexpr inline core::StrView extPartSv() const {
        core::StrView file = filePartSv();
        if (file.empty()) return {};

        addr_off dotRel = core::findLast(file.data(), file.len(), [](char c, addr_size) { return c == '.'; });
        if (dotRel < 0) return {}; // no ext part

        addr_size startRel = addr_size(dotRel + 1);
        auto ret = core::sv(file.data() + startRel, file.len() - startRel);
        return ret;
    }

    constexpr const char* dirPart() const {
        return dirPartSv().data();
    }
    constexpr inline core::StrView dirPartSv() const {
        addr_off lastSepIdx = core::findLast(buff, addr_size(len), [](char c, addr_size) { return c == core::PATH_SEPARATOR; });
        if (lastSepIdx < 0) return {};
        auto ret = core::sv(buff, addr_size(lastSepIdx + 1));
        return ret;
    }

    //==================================================================================================================
    // Set Parts
    //==================================================================================================================

    constexpr inline StaticPathBuilder& setDirPart(core::StrView directory) {
        if (directory.empty()) return *this;

        addr_off fOff = fileOff();
        if (buff[fOff] == core::PATH_SEPARATOR) {
            fOff++;
        }

        addr_size fileLen = addr_size(len) - addr_size(fOff);
        bool needsSep = directory.last() != core::PATH_SEPARATOR;
        addr_size newDirLen = directory.len() + (needsSep ? 1 : 0);
        addr_off newFileStart = addr_off(newDirLen);

        assertWriteSize(0, newDirLen + fileLen);

        // Preserve file part if set:
        if (fileLen > 0 && fOff != newFileStart) {
            if (newFileStart > fOff) {
                // Shift file part right:
                for (addr_size i = 0; i < fileLen; ++i) {
                    addr_size src = addr_size(fOff) + (fileLen - 1 - i);
                    addr_size dst = addr_size(newFileStart) + (fileLen - 1 - i);
                    buff[dst] = buff[src];
                }
            }
            else {
                // Shift file part left:
                for (addr_size i = 0; i < fileLen; ++i) {
                    addr_size src = addr_size(fOff) + i;
                    addr_size dst = addr_size(newFileStart) + i;
                    buff[dst] = buff[src];
                }
            }
        }

        core::memcopy(buff, directory.data(), directory.len());

        if (needsSep) {
            buff[directory.len()] = core::PATH_SEPARATOR;
        }

        len = i32(newDirLen + fileLen);
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    // This one is a bit of an adhoc hack. It overrides the file part !
    constexpr inline StaticPathBuilder& appendToDirPath(core::StrView dirName) {
        if (dirName.empty()) return *this;

        addr_off fOff = fileOff();
        if (buff[fOff] == core::PATH_SEPARATOR) {
            fOff++;
        }

        addr_size dirPrefixLen = addr_size(fOff);
        addr_size fileLen = addr_size(len) - dirPrefixLen;
        bool needsSep = dirName.last() != core::PATH_SEPARATOR;
        addr_size insertLen = dirName.len() + (needsSep ? 1 : 0);
        addr_off newFileStart = addr_off(dirPrefixLen + insertLen);

        assertWriteSize(0, addr_size(len) + insertLen);

        if (fileLen > 0 && insertLen > 0) {
            for (addr_size i = 0; i < fileLen; ++i) {
                addr_size src = dirPrefixLen + (fileLen - 1 - i);
                addr_size dst = addr_size(newFileStart) + (fileLen - 1 - i);
                buff[dst] = buff[src];
            }
        }

        core::memcopy(buff + dirPrefixLen, dirName.data(), dirName.len());
        if (needsSep) {
            buff[dirPrefixLen + dirName.len()] = core::PATH_SEPARATOR;
        }

        len += i32(insertLen);
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    constexpr inline StaticPathBuilder& setFilePart(core::StrView file) {
        if (file.empty()) {
            return *this;
        }

        addr_off fOff = fileOff();

        assertWriteSize(addr_size(fOff), file.len());

        if (buff[fOff] == core::PATH_SEPARATOR) {
            fOff++; // Skip the path separator
        }

        core::memcopy(buff + fOff, file.data(), file.len());

        len = i32(fOff) + i32(file.len());
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    constexpr inline StaticPathBuilder& setExtPart(core::StrView ext) {
        if (ext.empty()) return *this;

        addr_off eOff = extOff();

        assertWriteSize(addr_size(eOff), ext.len());

        // Add an extra dot if input str does not start with it and the current buffer does not end with it:
        if (ext.first() != '.') {
            if (buff[eOff] != '.') {
                buff[eOff] = '.';
            }
            eOff++; // skip dot
        }

        core::memcopy(buff + eOff, ext.data(), ext.len());

        len = i32(eOff) + i32(ext.len());
        buff[len] = '\0';

        debugClearBufferAfterLen();
        return *this;
    }

    constexpr inline void resetExtPart() {
        addr_off eOff = extOff();
        if (eOff >= len) {
            return; // no extension to reset
        }

        len = i32(eOff);
        buff[len] = '\0';

        debugClearBufferAfterLen();
    }

    //==================================================================================================================
    // Rest
    //==================================================================================================================

    constexpr inline void resetFilePart() {
        addr_off fOff = fileOff();
        if (fOff >= 0) {
            if (buff[fOff] == core::PATH_SEPARATOR) {
                fOff++;
                // This should not be possible, but sanity check it anyway:
                Panic(fOff < TBufferSize, "[BUG] StaticPathBuilder: buffer overflow.");
            }

            len = i32(fOff);
            buff[len] = '\0';
        }

        debugClearBufferAfterLen();
    }

    constexpr inline void reset() {
        len = 0;
        buff[0] = '\0';

        debugClearBufferAfterLen();
    }

private:
    constexpr inline void assertWriteSize(addr_size start, addr_size writeSize) {
        addr_size writeEndOff = start + writeSize;
        // Enforcing +2 because of the null terminator and a possible extra '\' or '.' symbols:
        Panic(writeEndOff + 2 < addr_size(TBufferSize), "StaticPathBuilder: buffer overflow");
    }

    constexpr inline addr_off fileOff() const { return const_cast<StaticPathBuilder*>(this)->fileOff(); }
    constexpr inline addr_off fileOff() {
        addr_off fOff = core::findLast(buff, addr_size(len), [](char x, addr_size) { return x == core::PATH_SEPARATOR; });
        if (fOff < 0) return 0;
        return fOff;
    }

    constexpr inline addr_off extOff() const { return const_cast<StaticPathBuilder*>(this)->extOff(); }
    constexpr inline addr_off extOff() {
        i32 fOff = i32(fileOff());
        addr_size fileLen = addr_size(len - i32(fOff));
        addr_off eOff = core::findLast(buff + fOff, fileLen, [](char x, addr_size) { return x == '.'; });
        if (eOff < 0) return len;
        return eOff + fOff;
    }

    constexpr inline void debugClearBufferAfterLen() {
#if CORE_DEBUG
        core::memset(buff + len + 1, '-', addr_size(TBufferSize - len - 1));
#endif
    }
};

static_assert(Path<StaticPathBuilder<255>>, "StaticPathBuilder does not satisfy Path concept");

} // namespace core
