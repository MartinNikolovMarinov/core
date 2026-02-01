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

template<i32 TBufferSize>
struct StaticPathBuilder {
    static_assert(TBufferSize > 0);

    char      buff[TBufferSize];
    addr_size len;

    constexpr inline const char* end() const { return buff + len; }
    constexpr inline char*       end()       { return buff + len; }

    constexpr inline const char* fullPath() const {
        return buff;
    }
    constexpr inline core::StrView fullPathSv() const {
        return { buff, len };
    }

    //==================================================================================================================
    // File Part
    //==================================================================================================================

    constexpr inline char* filePartMutable() {
        return const_cast<char*>(filePart());
    }
    constexpr inline const char* filePart() const {
        addr_off lastSepIdx = core::findLast(buff, len, [](char c, addr_size) { return c == PATH_SEPARATOR; });
        return buff + (lastSepIdx + 1); // This perfectly works without branching!
    }
    constexpr inline core::StrView filePartSv() const {
        addr_off lastSepIdx = core::findLast(buff, len, [](char c, addr_size) { return c == PATH_SEPARATOR; });
        addr_size start = addr_size(lastSepIdx + 1);
        return core::sv(buff + start, len - start);
    }

    //==================================================================================================================
    // Ext Part
    //==================================================================================================================

    constexpr inline char* extPartMutable() {
        return const_cast<char*>(extPart());
    }
    constexpr inline const char* extPart() const {
        const char* fp = filePart();
        addr_size fpLen = addr_size(this->end() - fp);
        addr_off dotRel = core::findLast(fp, fpLen, [](char c, addr_size) { return c == '.'; });
        if (dotRel < 0) return nullptr; // no ext part
        return fp + (dotRel + 1);
    }
    constexpr inline core::StrView extPartSv() const {
        const char* fp = filePart();
        addr_size fpLen = addr_size(this->end() - fp);
        addr_off dotRel = core::findLast(fp, fpLen, [](char c, addr_size) { return c == '.'; });
        if (dotRel < 0) return {}; // no ext part
        addr_size startRel = addr_size(dotRel + 1);
        return core::sv(fp + startRel, fpLen - startRel);
    }

    //==================================================================================================================
    // Dir Part
    //==================================================================================================================

    constexpr inline char* dirPartMutable() const {
        return const_cast<char*>(dirPart());
    }
    constexpr inline char* dirPart() const {
        addr_off lastSepIdx = core::findLast(buff, len, [](char c, addr_size) { return c == PATH_SEPARATOR; });
        if (lastSepIdx < 0) return nullptr;
        return buff;
    }
    constexpr inline core::StrView dirPartSv() const {
        addr_off lastSepIdx = core::findLast(buff, len, [](char c, addr_size) { return c == PATH_SEPARATOR; });
        if (lastSepIdx < 0) return {};
        return {buff, addr_size(lastSepIdx + 1)};
    }

    //==================================================================================================================
    // Set Parts
    //==================================================================================================================

    constexpr inline void setFilePart(const char* fileName) { return setFilePart(core::sv(fileName)); }
    constexpr inline void setFilePart(core::StrView fileName) {
        if (fileName.empty()) return;
        char* filep = filePartMutable();
        setPostfixPart(filep, fileName);
    }

    constexpr inline void setExtPart(const char* fileName) { return setExtPart(core::sv(fileName)); }
    constexpr inline void setExtPart(core::StrView fileName) {
        if (fileName.empty()) return;
        char* extp = extPartMutable();
        if (extp == nullptr) {
            Assert(len + 1 < addr_size(TBufferSize), "overflow");
            buff[len++] = '.';
            buff[len] = '\0';
            extp = buff + len;
        }
        setPostfixPart(extp, fileName);
    }

    // NOTE: setDirPart functions will overwrite the file and extension parts.
    // Call setDirPart* before setFilePart* and setExtPart*.
    constexpr inline void setDirPart(const char* dirName) { return setDirPart(core::sv(dirName)); }
    constexpr inline void setDirPart(core::StrView dirName) {
        if (dirName.empty()) return;
        Assert(dirName.len() + 1 < addr_size(TBufferSize), "overflow");
        char* start = buff;
        len = core::memcopy(start, dirName.data(), dirName.len());
        buff[len++] = PATH_SEPARATOR;
        buff[len] = '\0';
    }

    // NOTE: appendToDirPath functions will overwrite the file and extension parts.
    // Call setDirPart* before setFilePart* and setExtPart*.
    constexpr inline void appendToDirPath(const char* dirName) { return appendToDirPath(core::sv(dirName)); }
    constexpr inline void appendToDirPath(core::StrView dirName) {
        if (dirName.empty()) return;
        core::StrView dp = dirPartSv();
        addr_size newDirLen = dp.len() + dirName.len();
        Assert(newDirLen + 1 < addr_size(TBufferSize), "overflow");
        core::memcopy(buff + dp.len(), dirName.data(), dirName.len());
        len = newDirLen;
        buff[len++] = PATH_SEPARATOR;
        buff[len] = '\0';
    }

    //==================================================================================================================
    // Rest
    //==================================================================================================================

    constexpr inline void resetFilePart() {
        const char* fp = filePart();
        len = addr_size(fp - buff);
        buff[len] = '\0';
    }

    constexpr inline void reset() {
        core::memset(buff, char(0), sizeof(buff));
        len = 0;
    }

private:
    constexpr inline void setPostfixPart(char* ptr, core::StrView postfix) {
        if (postfix.empty()) return;

        char* start = buff;

        // Compute resulting length
        addr_size directoryPartLen = addr_size(ptr - start);
        addr_size newLen = directoryPartLen + postfix.len();

        // Need space for '\0'
        Assert(newLen + 1 < addr_size(TBufferSize), "overflow");

        core::memcopy(ptr, postfix.data(), postfix.len());
        len = newLen;
        buff[len] = '\0';
    }
};

static_assert(Path<StaticPathBuilder<255>>, "StaticPathBuilder does not satisfy Path concept");

} // namespace core
