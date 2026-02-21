#pragma once

// Expects: X(ctx, Name, CStr)
#define __CORE_ENUM_DECLARE_ITEM(_ctx, name, _cstrDesc) name,
#define __CORE_ENUM_DECLARE_CASE_FROM_DESC(ctx, name, cstrDesc) case (ctx name): return (cstrDesc);

// Generic case/return builder for X-macro expansion.
#define __CORE_ENUM_SWITCH_CASE_FROM_ENTRY(caseExpr, valueExpr) case (caseExpr): return (valueExpr);

/**
 * Declares a scoped enum with a fixed underlying type and generates a
 * constexpr enum-to-const-char* function named enum<EnumType>ToCstr.
 *
 * enumListMacro must be of the form:
 *     X(ctx, EnumValue, "Description")
 *
 * The list must include EnumValue == Undefined (and typically SENTINEL).
 * The conversion function falls back to enum<EnumType>ToCstr(EnumType::Undefined).
 *
 * @param enumType        Enum type name to declare.
 * @param underlyingType  Fixed underlying integral type of the enum.
 * @param enumListMacro   X-macro list providing (EnumValue, const char*) pairs.
 */
#define CORE_ENUM_DECLARE(enumType, underlyingType, enumListMacro)        \
    enum struct enumType : underlyingType {                               \
        enumListMacro(__CORE_ENUM_DECLARE_ITEM, _)                        \
    };                                                                    \
                                                                          \
    constexpr const char* enum##enumType##ToCstr(enumType value) {        \
        switch (value) {                                                  \
            enumListMacro(__CORE_ENUM_DECLARE_CASE_FROM_DESC, enumType::) \
            default:                                                      \
                return enum##enumType##ToCstr(enumType::Undefined);       \
        }                                                                 \
    }

/**
 * Declares a constexpr conversion function backed by a switch table.
 *
 * enumListMacro must emit entries in this form:
 *     X(caseExpr, returnExpr)
 *
 * Example:
 *     #define MY_MAP(X)                              \
 *         X(MyEnum::Undefined, MyValue::Undefined)   \
 *         X(MyEnum::A,         MyValue::First)
 *
 *     CORE_DECLARE_CONVERT_FN(toValue, MyEnum, MyValue, MyValue::Undefined, MY_MAP)
 *
 * @param fnName         Name of the generated function.
 * @param inputType      Type of function parameter used in switch(value).
 * @param returnType     Type returned by each case.
 * @param defaultReturn  Value returned in the default branch.
 * @param enumListMacro  X-macro list with (caseExpr, returnExpr) entries.
 */
#define CORE_DECLARE_CONVERT_FN(fnName, inputType, returnType, defaultReturn, enumListMacro) \
    constexpr returnType fnName(inputType value) {                                                \
        switch (value) {                                                                          \
            enumListMacro(__CORE_ENUM_SWITCH_CASE_FROM_ENTRY)                                     \
            default:                                                                              \
                return defaultReturn;                                                             \
        }                                                                                         \
    }

#define CORE_ENUM_DECLARE_INT_OPS(enumType, underlyingType)                    \
    constexpr underlyingType operator+(enumType value) {                       \
        return underlyingType(value);                                          \
    }                                                                          \
                                                                               \
    /* bitwise ops: enum <op> underlyingType and underlyingType <op> enum */   \
    constexpr bool operator==(enumType lhs, underlyingType rhs) {              \
        return underlyingType(lhs) == rhs;                                     \
    }                                                                          \
    constexpr bool operator==(underlyingType lhs, enumType rhs) {              \
        return lhs == underlyingType(rhs);                                     \
    }                                                                          \
    constexpr bool operator!=(enumType lhs, underlyingType rhs) {              \
        return underlyingType(lhs) != rhs;                                     \
    }                                                                          \
    constexpr bool operator!=(underlyingType lhs, enumType rhs) {              \
        return lhs != underlyingType(rhs);                                     \
    }                                                                          \
    constexpr bool operator<(enumType lhs, underlyingType rhs) {               \
        return underlyingType(lhs) < rhs;                                      \
    }                                                                          \
    constexpr bool operator<(underlyingType lhs, enumType rhs) {               \
        return lhs < underlyingType(rhs);                                      \
    }                                                                          \
    constexpr bool operator<=(enumType lhs, underlyingType rhs) {              \
        return underlyingType(lhs) <= rhs;                                     \
    }                                                                          \
    constexpr bool operator<=(underlyingType lhs, enumType rhs) {              \
        return lhs <= underlyingType(rhs);                                     \
    }                                                                          \
    constexpr bool operator>(enumType lhs, underlyingType rhs) {               \
        return underlyingType(lhs) > rhs;                                      \
    }                                                                          \
    constexpr bool operator>(underlyingType lhs, enumType rhs) {               \
        return lhs > underlyingType(rhs);                                      \
    }                                                                          \
    constexpr bool operator>=(enumType lhs, underlyingType rhs) {              \
        return underlyingType(lhs) >= rhs;                                     \
    }                                                                          \
    constexpr bool operator>=(underlyingType lhs, enumType rhs) {              \
        return lhs >= underlyingType(rhs);                                     \
    }                                                                          \
                                                                               \
    /* bitwise ops: enum <op> enum */                                          \
    constexpr enumType operator~(enumType v) {                                 \
        return enumType(                                                       \
            underlyingType(~underlyingType(v)));                               \
    }                                                                          \
    constexpr enumType operator|(enumType a, enumType b) {                     \
        return enumType(                                                       \
            underlyingType(a) | underlyingType(b));                            \
    }                                                                          \
    constexpr enumType operator&(enumType a, enumType b) {                     \
        return enumType(                                                       \
            underlyingType(a) & underlyingType(b));                            \
    }                                                                          \
    constexpr enumType operator^(enumType a, enumType b) {                     \
        return enumType(                                                       \
            underlyingType(a) ^ underlyingType(b));                            \
    }                                                                          \
    constexpr enumType& operator|=(enumType& a, enumType b) {                  \
        a = (a | b);                                                           \
        return a;                                                              \
    }                                                                          \
    constexpr enumType& operator&=(enumType& a, enumType b) {                  \
        a = (a & b);                                                           \
        return a;                                                              \
    }                                                                          \
    constexpr enumType& operator^=(enumType& a, enumType b) {                  \
        a = (a ^ b);                                                           \
        return a;                                                              \
    }                                                                          \
                                                                               \
    /* bitwise ops: enum <op> underlyingType */                                \
    constexpr underlyingType operator|(enumType a, underlyingType b) {         \
        return underlyingType(a) | b;                                          \
    }                                                                          \
    constexpr underlyingType operator|(underlyingType a, enumType b) {         \
        return a | underlyingType(b);                                          \
    }                                                                          \
    constexpr underlyingType operator&(enumType a, underlyingType b) {         \
        return underlyingType(a) & b;                                          \
    }                                                                          \
    constexpr underlyingType operator&(underlyingType a, enumType b) {         \
        return a & underlyingType(b);                                          \
    }                                                                          \
    constexpr underlyingType operator^(enumType a, underlyingType b) {         \
        return underlyingType(a) ^ b;                                          \
    }                                                                          \
    constexpr underlyingType operator^(underlyingType a, enumType b) {         \
        return a ^ underlyingType(b);                                          \
    }
