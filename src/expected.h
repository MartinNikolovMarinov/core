#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core
{

using namespace coretypes;

template <typename T, typename TErr>
struct CORE_API_EXPORT Expected {

    constexpr Expected(T&& value)  : value(core::Move(value)), hasValue(true) {}
    constexpr Expected(TErr&& err) : err(core::Move(err)), hasValue(false) {}

    // no copy
    Expected(const Expected&) = delete;
    Expected& operator=(const Expected&) = delete;

    constexpr Expected(Expected&& other) : hasValue(other.hasValue) {
        if (other.HasValue()) value = core::Move(other.value);
        else err = core::Move(other.err);
    }

    ~Expected() {
        if (HasValue()) value.~T();
        else err.~TErr();
    }

    constexpr bool HasValue() const { return hasValue; }
    constexpr bool HasErr()   const { return !hasValue; }

    constexpr const T& Value()  const { return value; }
    constexpr T& Value()              { return value; }
    constexpr const TErr& Err() const { return err; }
    constexpr TErr& Err()             { return err; }

    constexpr T&& ValueOrDie() {
        Assert(HasValue(), "Expected::ValueOrDie() called on an Expected with an error");
        return core::Move(value);
    }

private:
    union { T value; TErr err; };
    const bool hasValue;
};

#define ValueOrReturn(expr, v) {      \
    auto __ret = (expr);              \
    if (__ret.HasErr()) return __ret; \
    (v) = __ret.Value();              \
}

} // namespace core
