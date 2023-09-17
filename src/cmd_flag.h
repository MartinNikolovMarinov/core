#pragma once

#include <types.h>
#include <arr.h>
#include <expected.h>
#include <char_ptr.h>
#include <char_ptr_conv.h>
#include <algorithms.h>
#include <str_builder.h>
#include <hash_map.h>

// FIXME: Might want to consider copying the character pointers instead of trusting the user to keep them alive.
//        This is usually fine because the parser has a very specific use case, but might bring about some terrible bugs.
//        Possible solution - simply copy all character pointers for flag names and aliases in a large char buffer.
//        Use the destructor to free the memory block.

// FIXME: Test more extensively the aliasing feature. There are a billion edge cases that need to be tested.

namespace core {

using namespace coretypes;

template<typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct flag_parser {
    static constexpr i32 MAX_ARG_LEN = 5000;
    static constexpr i32 MAX_FLAG_COUNT = 100;

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
        core::str_view name = sv();
        flag_type type;
        bool isSet = false;
        bool isRequired = false;
        ValidationFn validate = nullptr;
    };

    struct alias {
        const core::str_view* name = nullptr;
        core::str_view alias = sv();
    };

    enum struct parse_err {
        Success = 0,
        ArgLenTooLong = 1,
        TooManyFlags = 2,
        UnknownFlag = 3,
        MissingRequiredFlag = 4,
        NothingToParse = 5,
        CustomRuleViolation = 6
    };

    core::arr<flag_data, TAllocator> flags;
    core::hash_map<core::str_view, flag_data*, TAllocator> aliases;
    i32 maxArgLen;
    bool allowUnknownFlags;

    flag_parser() : flags(), maxArgLen(MAX_ARG_LEN), allowUnknownFlags(false) {}

    /**
     * \brief Parses the given arguments and sets the flags accordingly. If argv is modified or freed the parser will
     *        break. The lifetime of argv must be as long as the parser. This is an odd interface, but it uses very
     *        little memory, and besides, argv is usually global and it usually is never changed, or freed.
     */
    core::expected<parse_err> parse(i32 argc, const char** argv) {
        i32 state = 0;
        flag_parser::flag_data* curFlag = nullptr;

        if (argv == nullptr) return core::unexpected(parse_err::NothingToParse);
        if (argc == 0) return core::unexpected(parse_err::NothingToParse);
        if (argc > MAX_FLAG_COUNT) return core::unexpected(parse_err::TooManyFlags);

        for (i32 i = 0; i < argc; ++i) {
            const char* curVal = argv[i];

            // Skip empty arguments and -, -- at the beginning of flags.
            if (curVal == nullptr) continue;
            if (state != 2) curVal = core::cptr_skip_space(curVal);
            if (curVal[0] == '-' && state == 0) {
                curVal++;
                state = 1;
                if (curVal[0] == '-') curVal++; // allow long -- flags
            }

            // Check the length of the flag:
            i32 valLen = 0;
            while (curVal[valLen]) {
                valLen++;
                if (valLen > maxArgLen) {
                    return core::unexpected(parse_err::ArgLenTooLong);
                }
            }

            if (state == 1) {
                auto trimmedEq = [&](const core::str_view a, const core::str_view b) -> bool {
                    addr_off trimmedALen = addr_off(a.len);
                    addr_off trimmedBLen = addr_off(b.len);
                    while (core::is_white_space(a.data()[trimmedALen - 1])) trimmedALen--;
                    while (core::is_white_space(b.data()[trimmedBLen - 1])) trimmedBLen--;
                    bool areEqual = core::cptr_eq(a.data(), b.data(), addr_size(core::min(trimmedALen, trimmedBLen)));
                    return areEqual;
                };

                addr_off fidx = 0;

                // Try to find the flag by name:
                fidx = core::find(flags, [&](const flag_data& f, addr_off) -> bool {
                    return trimmedEq(f.name, sv(curVal, addr_size(valLen)));
                });

                if (fidx == -1) {
                    // Try to find the flag by some alias:
                    flag_data** flagData = aliases.get(sv(curVal, addr_size(valLen)));
                    if (flagData != nullptr) {
                        Assert((*flagData), "[BUG] Alias points to nullptr flag data.");
                        fidx = core::find(flags, [&](const flag_data& f, addr_off) -> bool {
                            return trimmedEq(f.name, (*flagData)->name);
                        });
                        Assert(fidx != -1, "[BUG] Alias points to non-existing flag.");
                    }
                }

                if (fidx == -1 && !allowUnknownFlags) {
                    return core::unexpected(parse_err::UnknownFlag);
                }

                if (fidx != -1) curFlag = &flags[addr_size(fidx)];
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

        for (addr_size i = 0; i < flags.len(); ++i) {
            const auto& f = flags[i];
            if (f.isRequired && !f.isSet) {
                return core::unexpected(parse_err::MissingRequiredFlag);
            }
            if (f.isSet && f.validate && !f.validate(f.arg)) {
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

    void alias(const char* flagName, const char* alias) {
        addr_off fidx = core::find(flags, [&](const flag_data& f, addr_off) -> bool {
            return f.name == flagName;
        });
        if (fidx != -1) {
            auto& f = flags[fidx];
            auto* aliasList = aliases.get(f.name);
            if (aliasList == nullptr) {
                aliases.insert(f.name, core::arr<str_view, TAllocator>());
                aliasList = aliases.get(f.name);
                Panic(aliasList != nullptr, "[BUG] Failed to insert alias list.");
            }
            aliasList->append(sv(core::cptr_skip_space(alias), core::cptr_len(alias)));
        }
        Assert(fidx != -1, "Can't create alias for non-existing flag.");
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
        f.name = sv(core::cptr_skip_space(flagName), core::cptr_len(flagName));
        f.type = type;
        f.isSet = false;
        f.isRequired = required;
        f.validate = validate;

        // Replace existing flag if it exists.
        addr_off nameIdx = core::find(parser.flags, [&](const auto& el, addr_off) -> bool {
            return f.name == el.name;
        });
        if (nameIdx != -1) {
            // FIXME: Update aliases!
            parser.flags[addr_size(nameIdx)] = f;
        }
        else {
            parser.flags.append(f);
        }
    }
};

} // namespace core
