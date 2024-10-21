#pragma once

#include <core_alloc.h>
#include <core_API.h>
#include <core_arr.h>
#include <core_cstr_conv.h>
#include <core_cstr.h>
#include <core_expected.h>
#include <core_hash_map.h>
#include <core_hash.h>
#include <core_mem.h>
#include <core_str_builder.h>
#include <core_str_view.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT CmdFlagParser;

template<> addr_size hash<StrView>(const StrView&);
template<> bool eq<StrView>(const StrView&, const StrView&);

struct CORE_API_EXPORT CmdFlagParser {
    struct ParsedSymbol;
    struct FlagData;

    using ParsedSymbols = ArrList<ParsedSymbol>;
    using FlagDataMap = HashMap<StrView, FlagData>;
    using FlagValidationFn =  bool (*)(void* val);

    static constexpr u32 MAX_ARG_COUNT = 100;

    enum struct ParseError : u8 {
        None,

        NothingToParse,
        ArgumentListTooLong,
        InvalidArgvElement,
        IncorrectValue,
        FlagWithoutValue,

        SENTINEL
    };

    enum struct MatchError : u8 {
        None,

        NoSymbolsToMatch,
        MissingRequiredFlag,
        ValidationFailed,
        UnknownFlag,
        FaildToParseNumber,

        SENTINEL
    };

    enum struct ParsedSymbolType : u8 {
        None,

        ProgramName,
        Argument,
        FlagName,
        FlagValue,
        OptionName,

        SENTINEL
    };

    struct ParsedSymbol {
        ParsedSymbolType type;
        StrBuilder value;
    };

    enum struct FlagType {
        None,

        Bool,
        Int32,
        Int64,
        Uint32,
        Uint64,
        Float32,
        Float64,
        String,

        SENTINEL
    };

    struct FlagData {
        StrBuilder name;
        FlagType type = FlagType::None;
        void* data = nullptr;
        bool isSet = false;
        bool isRequired = false;
        FlagValidationFn validationFn = nullptr;
    };

    void allowUnknownFlags(bool allow) {
        m_allowUnknownFlags = allow;
    }

    core::expected<ParseError> parse(addr_size argc, const char** argv) {
        if (argv == nullptr) return core::unexpected(ParseError::NothingToParse);
        if (argc == 0) return core::unexpected(ParseError::NothingToParse);
        if (argc > MAX_ARG_COUNT) return core::unexpected(ParseError::ArgumentListTooLong);

        // Pre-allocate at enough space for the worst case scenario:
        m_parsedSymbols.clear();
        m_parsedSymbols.ensureCap(argc*2);
        m_argumentCount = 0;

        auto trailingWhitespaceIdx = [](const char* str, addr_size len) -> addr_size {
            if (!core::isWhiteSpace(str[len - 1])) return len;
            addr_size idx = len;
            while (idx > 0 && core::isWhiteSpace(str[idx - 1])) {
                idx--;
            }
            return idx;
        };

        i32 state = 0;
        bool isValue = false;

        for (addr_size i = 0; i < argc; i++) {
            const char* chunk = argv[i];
            if (chunk == nullptr) return core::unexpected(ParseError::InvalidArgvElement);

            chunk = core::cstrSkipSpace(chunk);

            bool isFlag = false;
            bool isOption = false;
            if (chunk[0] == '-' && isValue == false) {
                chunk++; // skip first -
                if (chunk[0] == '-') {
                    chunk++; // skip second -
                    isOption = true;
                }
                else {
                    isFlag = true;
                }
            }

            addr_size trailingIdx = trailingWhitespaceIdx(chunk, core::cstrLen(chunk));

            switch (state) {
                case 0: // in program name
                {
                    ParsedSymbol s = { ParsedSymbolType::ProgramName, StrBuilder(core::sv(chunk, trailingIdx)) };
                    m_parsedSymbols.push(std::move(s));
                    state = 1;
                    break;
                }

                case 1: // in argument list
                {
                    if (!isFlag && !isOption) {
                        ParsedSymbol s = { ParsedSymbolType::Argument, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                        m_argumentCount++;
                    }
                    else if (isFlag) {
                        ParsedSymbol s = { ParsedSymbolType::FlagName, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                        isValue = true; // next symbol is a value.
                        state = 2; // end of argument list, found first flag.
                    }
                    else {
                        ParsedSymbol s = { ParsedSymbolType::OptionName, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                        state = 2; // end of argument list, found first option.
                    }

                    break;
                }

                case 2:
                {
                    if (isFlag) {
                        ParsedSymbol s = { ParsedSymbolType::FlagName, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                        isValue = true; // next symbol is a value.
                    }
                    else if (isOption && !isValue) {
                        ParsedSymbol s = { ParsedSymbolType::OptionName, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                    }
                    else if (isValue) {
                        ParsedSymbol s = { ParsedSymbolType::FlagValue, StrBuilder(core::sv(chunk, trailingIdx)) };
                        m_parsedSymbols.push(std::move(s));
                        isValue = false;
                    }
                    else {
                        return core::unexpected(ParseError::IncorrectValue);
                    }

                    break;
                }

                default:
                {
                    Panic(false, "Implementation bug: invalid state");
                }
            };
        }

        if (m_parsedSymbols.last().type == ParsedSymbolType::FlagName) {
            // Dangling flag name without a value.
            return core::unexpected(ParseError::FlagWithoutValue);
        }

        return {};
    }

    const ParsedSymbols& parsedSymbols() const {
        return m_parsedSymbols;
    }

    StrView programName() const {
        StrView ret = !m_parsedSymbols.empty() ? m_parsedSymbols[0].value.view() : sv();
        return ret;
    }

    inline u32 argumentCount() const {
        return m_argumentCount;
    }

    template <typename TCallback>
    inline void arguments(TCallback cb) const {
        for (addr_size i = 1; i < m_parsedSymbols.len(); i++) {
            auto& s = m_parsedSymbols[i];
            if (s.type == ParsedSymbolType::Argument) {
                if (!cb(s.value.view(), i - 1)) {
                    return;
                }
            }
        }
    }

    template <typename TCallback>
    inline void flags(TCallback cb) const {
        for (addr_size i = 1; i < m_parsedSymbols.len(); i++) {
            auto& s = m_parsedSymbols[i];
            if (s.type == ParsedSymbolType::FlagName) {
                const ParsedSymbol* next = (i + 1 < m_parsedSymbols.len()) ? &m_parsedSymbols[i + 1] : nullptr;
                StrView nextValue = next ? next->value.view() : sv();
                if (!cb(s.value.view(), nextValue)) {
                    return;
                }
            }
        }
    }

    template <typename TCallback>
    inline void options(TCallback cb) const {
        for (addr_size i = 1; i < m_parsedSymbols.len(); i++) {
            auto& s = m_parsedSymbols[i];
            if (s.type == ParsedSymbolType::OptionName) {
                if (!cb(s.value.view())) {
                    return;
                }
            }
        }
    }

    core::expected<MatchError> matchFlags() {
        if (m_parsedSymbols.empty()) return core::unexpected(MatchError::NoSymbolsToMatch);

        MatchError err = MatchError::None;

        // Reset the flag data:

        m_flagData.values([](FlagData& fd) -> bool {
            fd.isSet = false;
            return true;
        });

        // Match the flags:

        flags([&, this](core::StrView flag, core::StrView value) -> bool {
            if (auto fd = m_flagData.get(flag); fd) {
                switch (fd->type) {
                    case FlagType::Bool:
                    {
                        addr_size vLen = value.len();
                        bool v = false;

                        if (vLen >= 4) {
                            v = core::memcmp(value.data(), 4, "true", 4) == 0 ||
                                core::memcmp(value.data(), 4, "True", 4) == 0 ||
                                core::memcmp(value.data(), 4, "TRUE", 4) == 0;
                            if (vLen > 4) {
                                // This checks that the words ['true', 'True', 'TRUE'] are not a prefix of a longer word.
                                v &= core::isWhiteSpace(value[4]) || value[4] == '\0';
                            }
                        }
                        else if (vLen >= 1) {
                            v = value[0] == '1' || value[0] == 't' || value[0] == 'T';
                            if (vLen > 1) {
                                // This checks that the symbols ['1', 't', 'T'] are not a prefix of a longer word.
                                v &= core::isWhiteSpace(value[1]) || value[1] == '\0';
                            }
                        }

                        *reinterpret_cast<bool*>(fd->data) = v;
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Int32:
                    {
                        i32 v = core::cstrToInt<i32>(value.data());
                        *reinterpret_cast<i32*>(fd->data) = v;
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Int64:
                    {
                        i64 v = core::cstrToInt<i64>(value.data());
                        *reinterpret_cast<i64*>(fd->data) = v;
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Uint32:
                    {
                        u32 v = core::cstrToInt<u32>(value.data());
                        *reinterpret_cast<u32*>(fd->data) = v;
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Uint64:
                    {
                        u64 v = core::cstrToInt<u64>(value.data());
                        *reinterpret_cast<u64*>(fd->data) = v;
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Float32:
                    {
                        auto v = core::cstrToFloat<f32>(value.data(), value.len());
                        if (v.hasErr()) {
                            err = MatchError::FaildToParseNumber;
                            return false;
                        }
                        *reinterpret_cast<f32*>(fd->data) = v.value();
                        fd->isSet = true;
                        break;
                    }
                    case FlagType::Float64:
                    {
                        // FIXME: Uncomment this when fixed.
                        // auto v = core::cstrToFloat<f64>(value.data(), value.len());
                        // if (v.hasErr()) {
                        //     err = MatchError::FaildToParseNumber;
                        //     return false;
                        // }
                        // *reinterpret_cast<f64*>(fd->data) = v.value();
                        // fd->isSet = true;
                        break;
                    }
                    case FlagType::String:
                    {
                        auto v = reinterpret_cast<StrBuilder*>(fd->data);
                        v->clear();
                        v->append(core::sv(value.data()));
                        fd->isSet = true;
                        break;
                    }

                    case FlagType::None:     [[fallthrough]];
                    case FlagType::SENTINEL: Panic(false, "Implementation bug: invalid flag type");
                }
            }
            else {
                if (!m_allowUnknownFlags) {
                    err = MatchError::UnknownFlag;
                    return false;
                }
            }

            return true;
        });

        if (err != MatchError::None) {
            return core::unexpected(err);
        }

        // Run any validation functions:

        m_flagData.values([&](FlagData& fd) -> bool {
            if (fd.isRequired && !fd.isSet) {
                err = MatchError::MissingRequiredFlag;
                return false;
            }

            if (fd.isSet && fd.validationFn) {
                if (!fd.validationFn(fd.data)) {
                    err = MatchError::ValidationFailed;
                    return false;
                }
            }
            return true;
        });

        if (err != MatchError::None) {
            return core::unexpected(err);
        }

        return {};
    }

    void setFlagString(StrBuilder* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::String);
    }

    void setFlagBool(bool* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Bool);
    }

    void setFlagInt32(i32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Int32);
    }

    void setFlagInt64(i64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Int64);
    }

    void setFlagUint32(u32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Uint32);
    }

    void setFlagUint64(u64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Uint64);
    }

    void setFlagFloat32(f32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Float32);
    }

    void setFlagFloat64(f64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr) {
        _insertFlag(out, flagName, required, validation, FlagType::Float64);
    }

    void alias(StrView flagName, StrView aliasName) {
        FlagData* existing = m_flagData.get(flagName);
        if (existing) {
            FlagData fd;
            fd.name = StrBuilder(aliasName);
            fd.type = existing->type;
            fd.data = existing->data;
            fd.isSet = existing->isSet;
            fd.isRequired = existing->isRequired;
            fd.validationFn = existing->validationFn;

            auto key = fd.name.view();
            m_flagData.put(key, std::move(fd));
        }
        else {
            Panic(false, "Alias target does not exist");
        }
    }

private:
    template <typename T>
    void _insertFlag(T* out, StrView flagName, bool required, FlagValidationFn validation, FlagType type) {
        FlagData* existing = m_flagData.get(flagName);
        if (existing) {
            Panic(false, "Inserting duplicate flags is not allowed");
        }

        FlagData fd;
        fd.name = core::StrBuilder(flagName);
        fd.type = type;
        fd.data = out;
        fd.isSet = false;
        fd.isRequired = required;
        fd.validationFn = validation;

        auto key = fd.name.view();
        m_flagData.put(key, std::move(fd));
    }

    ParsedSymbols m_parsedSymbols;
    FlagDataMap m_flagData;
    bool m_allowUnknownFlags = false;
    u32 m_argumentCount = 0;
};

} // namespace core
