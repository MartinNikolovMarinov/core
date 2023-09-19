#pragma once

#include <types.h>
#include <arr.h>
#include <expected.h>
#include <char_ptr.h>
#include <char_ptr_conv.h>
#include <algorithms.h>
#include <hash_map.h>
#include <str_builder.h>

// FIXME: Might want to consider copying the character pointers instead of trusting the user to keep them alive.
//        This is usually fine because the parser has a very specific use case, but might bring about some terrible bugs.
//        Possible solution - simply copy all character pointers for flag names and aliases in a large char buffer.
//        Use the destructor to free the memory block.

// FIXME: Test more extensively the aliasing feature. There are a billion edge cases that need to be tested.

// NOTE: Do not expect thread safety from this code.

namespace core {

using namespace coretypes;

template<typename TAllocator = CORE_DEFAULT_ALLOCATOR()>
struct flag_parser {
    using sb = core::str_builder<TAllocator>;

    static constexpr i32 MAX_ARG_LEN = 5000;
    static constexpr i32 MAX_FLAG_COUNT = 100;

    enum struct flag_type {
        None,

        Bool,
        Int32,
        Int64,
        Uint32,
        Uint64,
        Float32,
        Float64,
        CPtr,

        SENTINEL
    };

    using ValidationFn = bool (*)(void* val);

    struct flag_data {
        void* arg = nullptr;
        sb nameSb;
        flag_type type = flag_type::None;
        bool isSet = false;
        bool isRequired = false;
        ValidationFn validate = nullptr;
    };

    struct flag_alias {
        sb aliasSb;
        flag_data* flag;
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
    core::hash_map<core::str_view, flag_alias, TAllocator> aliases;
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
                    addr_size startA = 0;
                    addr_size startB = 0;
                    addr_size endA = a.len();
                    addr_size endB = b.len();

                    while (startA < endA && core::is_white_space(a.data()[startA])) startA++;
                    while (startB < endB && core::is_white_space(b.data()[startB])) startB++;
                    while (endA > startA && core::is_white_space(a.data()[endA - 1])) endA--;
                    while (endB > startB && core::is_white_space(b.data()[endB - 1])) endB--;

                    bool areEqual = core::cptr_cmp(a.data() + startA, endA - startA, b.data() + startB, endB - startB) == 0;
                    return areEqual;
                };

                addr_off fidx = 0;

                // Try to find the flag by name:
                fidx = core::find(flags, [&](const flag_data& f, addr_off) -> bool {
                    return trimmedEq(f.nameSb.view(), sv(curVal, addr_size(valLen)));
                });

                if (fidx == -1) {
                    // Try to find the flag by some alias:
                    flag_alias* as = aliases.get(sv(curVal, addr_size(valLen)));
                    if (as != nullptr) {
                        Assert(as->flag, "[BUG] Alias points to nullptr flag data.");
                        curFlag = as->flag;
                        state = 2;
                        continue;
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

                    case flag_type::None:     [[fallthrough]];
                    case flag_type::SENTINEL: [[fallthrough]];
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

    void flag(char** out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::CPtr);
    }

    void flag(i32* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Int32);
    }

    void flag(i64* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Int64);
    }

    void flag(u32* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Uint32);
    }

    void flag(u64* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Uint64);
    }

    void flag(bool* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Bool);
    }

    void flag(f32* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Float32);
    }

    void flag(f64* out, str_view flagName, bool required = false, ValidationFn validate = nullptr) {
        return _flag(*this, out, flagName, required, validate, flag_type::Float64);
    }

    void alias(str_view flagName, str_view alias) {
        addr_off fidx = core::find(flags, [&](const flag_data& f, addr_off) -> bool {
            return core::cptr_eq(f.nameSb.view().data(), flagName.data(), flagName.len());
        });

        Assert(fidx != -1, "Can't create an alias for non-existing flag.");
        auto& f = flags[addr_size(fidx)];
        flag_alias a;
        a.aliasSb = alias; // makes a copy
        a.flag = &f;
        aliases.put(a.aliasSb.view(), core::move(a)); // The alias holds it's own copy of the name in an sb instance.
    }

private:
    template <typename T>
    static void _flag(flag_parser& parser,
                      T* out,
                      str_view flagName,
                      bool required,
                      ValidationFn validate,
                      flag_parser::flag_type type) {
        flag_parser::flag_data f;
        f.arg = (void*)out;
        f.nameSb = flagName; // makes a copy
        f.type = type;
        f.isSet = false;
        f.isRequired = required;
        f.validate = validate;

        // Replace existing flag if it exists.
        addr_off nameIdx = core::find(parser.flags, [&](const auto& el, addr_off) -> bool {
            return f.nameSb.eq(el.nameSb);
        });
        if (nameIdx != -1) {
            flag_data* oldFlagPtr = &parser.flags[addr_size(nameIdx)];
            parser.flags[addr_size(nameIdx)] = core::move(f);
            flag_data* newFlagPtr = &parser.flags[addr_size(nameIdx)];

            // Update aliases to point to the new flag.
            parser.aliases.values([&](flag_alias& alias) {
                if (alias.flag == oldFlagPtr) {
                    alias.flag = newFlagPtr;
                }
            });
        }
        else {
            parser.flags.append(core::move(f));
        }
    }
};

} // namespace core
