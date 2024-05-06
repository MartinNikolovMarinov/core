#pragma once

#include <core_alloc.h>
#include <core_API.h>
#include <core_arr.h>
#include <core_cptr_conv.h>
#include <core_cptr.h>
#include <core_expected.h>
#include <core_hash_map.h>
#include <core_str_builder.h>
#include <core_str_view.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

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

    void allowUnknownFlags(bool allow);

    core::expected<ParseError> parse(addr_size argc, const char** argv);

    const ParsedSymbols& parsedSymbols() const;

    StrView programName() const;

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

    core::expected<MatchError> matchFlags();

    void setFlagString(StrBuilder* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagBool(bool* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagInt32(i32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagInt64(i64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagUint32(u32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagUint64(u64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagFloat32(f32* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);
    void setFlagFloat64(f64* out, StrView flagName, bool required = false, FlagValidationFn validation = nullptr);

    void alias(StrView flagName, StrView aliasName);

private:
    ParsedSymbols m_parsedSymbols;
    FlagDataMap m_flagData;
    bool m_allowUnknownFlags = false;
    u32 m_argumentCount = 0;
};

} // namespace core
