#pragma once

#include <types.h>
#include <arr.h>
#include <expected.h>
#include <char_ptr.h>
#include <char_ptr_conv.h>
#include <algorithms.h>

namespace core {

using namespace coretypes;

template<typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct flag_parser {
    static constexpr i32 MAX_ARG_LEN = 5000;

    enum flag_type {
        Bool,
        Int32,
        Int64,
        Uint32,
        Uint64,
        Float32,
        Float64,
        CPtr,
    };

    using ValidationFn = bool (*)(void* val);

    struct flag_data {
        void* arg = nullptr;
        const char* name = nullptr;
        i32 nameLen = 0;
        flag_type type;
        bool isSet = false;
        bool isRequired = false;
        ValidationFn validate;
    };

    enum struct parse_err {
        Success = 0,
        ArgLenTooLong = 1,
        UnknownFlag = 2,
        MissingRequiredFlag = 3,
        NothingToParse = 4,
        CustomRuleViolation = 5
    };

    core::arr<flag_data, TAllocator> flags;
    i32 maxArgLen;
    bool allowUnknownFlags;

    flag_parser() : flags(), maxArgLen(MAX_ARG_LEN), allowUnknownFlags(false) {}

    /**
     * \brief Parses the given arguments and sets the flags accordingly. If argv is modified or freed the parser will
     *        break. The lifetime of argv must be as long as the parser. This is an odd interface, but it uses very
     *        little memory, and besides, argv is usually global and it usually is never changed, or freed.
     *
     * \param argc The number of arguments in argv.
     * \param argv The arguments to parse.
     */
    core::expected<parse_err> parse(i32 argc, const char** argv) {
        i32 state = 0;
        flag_parser::flag_data* curFlag = nullptr;

        if (argv == nullptr) return core::unexpected(parse_err::NothingToParse);
        if (argc == 0) return core::unexpected(parse_err::NothingToParse);

        for (i32 i = 0; i < argc; ++i) {
            const char* curVal = argv[i];
            if (curVal == nullptr) continue;
            if (state != 2) curVal = core::cptr_skip_space(curVal);
            if (curVal[0] == '-' && state == 0) {
                curVal++;
                state = 1;
                if (curVal[0] == '-') curVal++; // allow long -- flags
            }
            i32 valLen = 0;
            while (curVal[valLen]) {
                valLen++;
                if (valLen > maxArgLen) {
                    return core::unexpected(parse_err::ArgLenTooLong);
                }
            }

            if (state == 1) {
                i32 fidx = core::find(flags, [&](const flag_data& f, i32) -> bool {
                    i32 trimmedNameLen = f.nameLen;
                    i32 trimmedValLen = valLen;
                    while (core::is_white_space(f.name[trimmedNameLen - 1])) trimmedNameLen--;
                    while (core::is_white_space(curVal[trimmedValLen - 1])) trimmedValLen--;
                    bool areEqual = core::cptr_cmp(f.name, trimmedNameLen, curVal, trimmedValLen) == 0;
                    return areEqual;
                });

                if (fidx == -1 && !allowUnknownFlags) {
                    return core::unexpected(parse_err::UnknownFlag);
                }

                if (fidx != -1) curFlag = &flags[fidx];
                state = 2;
            }
            else if (state == 2) {
                state = 0;
                if (curFlag == nullptr) continue; // maybe unknown flag, or just some random argument that is not a flag.
                switch (curFlag->type) {
                    case flag_type::Bool:
                    {
                        const char* skipped = core::cptr_skip_space(curVal);
                        i32 trimmedLen = valLen + i32(curVal - skipped);
                        while (core::is_white_space(skipped[trimmedLen - 1])) trimmedLen--;
                        bool v = skipped[0] == '1';
                        if (!v) {
                            if (trimmedLen == 1) {
                                v = skipped[0] == 't' || skipped[0] == 'T';
                            }
                            else if (trimmedLen == 4) {
                                v = core::cptr_cmp(skipped, 4, "true", 4) == 0 ||
                                    core::cptr_cmp(skipped, 4, "True", 4) == 0 ||
                                    core::cptr_cmp(skipped, 4, "TRUE", 4) == 0;
                            }
                        }
                        *(bool*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Int32:
                    {
                        i32 v = core::cptr_to_int<i32>(curVal);
                        *(i32*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Int64:
                    {
                        i64 v = core::cptr_to_int<i64>(curVal);
                        *(i64*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Uint32:
                    {
                        u32 v = core::cptr_to_int<u32>(curVal);
                        *(u32*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Uint64:
                    {
                        u64 v = core::cptr_to_int<u64>(curVal);
                        *(u64*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Float32:
                    {
                        f32 v = core::cptr_to_float<f32>(curVal);
                        *(f32*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::Float64:
                    {
                        f64 v = core::cptr_to_float<f64>(curVal);
                        *(f64*)curFlag->arg = v;
                        curFlag->isSet = true;
                        break;
                    }
                    case flag_type::CPtr:
                    {
                        *(const char**)curFlag->arg = curVal;
                        curFlag->isSet = true;
                        break;
                    }
                    default:
                    {
                        if (allowUnknownFlags) break;
                        return core::unexpected(parse_err::UnknownFlag);
                    }
                }
            }
        }

        for (i32 i = 0; i < flags.len(); ++i) {
            const auto& f = flags[i];
            if (f.isRequired && !f.isSet) {
                return core::unexpected(parse_err::MissingRequiredFlag);
            }
            if (f.validate && !f.validate(f.arg)) {
                return core::unexpected(parse_err::CustomRuleViolation);
            }
        }

        return {};
    }

    void flag(char** out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::CPtr);
    }

    void flag(i32* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Int32);
    }

    void flag(i64* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Int64);
    }

    void flag(u32* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Uint32);
    }

    void flag(u64* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Uint64);
    }

    void flag(bool* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Bool);
    }

    void flag(f32* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Float32);
    }

    void flag(f64* out, const char* flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Float64);
    }

private:
    template <typename T>
    static void _flag(flag_parser& parser,
                      T* out,
                      const char* flagName,
                      bool required,
                      ValidationFn validate,
                      flag_parser::flag_type type) {
        flag_parser::flag_data f;
        f.arg = (void*)out;
        f.name = core::cptr_skip_space(flagName);
        f.nameLen = core::cptr_len(f.name);
        f.type = type;
        f.isSet = false;
        f.isRequired = required;
        f.validate = validate;

        // Replace existing flag if it exists.
        i32 nameIdx = core::find(parser.flags, [&](const auto& el, i32) -> bool {
            return core::cptr_cmp(f.name, f.nameLen, el.name, el.nameLen) == 0;
        });
        if (nameIdx != -1) parser.flags[nameIdx] = f;
        else parser.flags.append(f);
    }
};

} // namespace core
