// Copyright (c) 2020 by the Zeek Project. See LICENSE for details.

#pragma once

#include <hilti/rt/exception.h>

#include <hilti/3rdparty/SafeInt/SafeInt.hpp>

namespace hilti::rt {

/**
 * Exception triggered when a numerical operation causes an overflow.
 */
HILTI_EXCEPTION(Overflow, RuntimeError)

/**
 * Exception triggered when a division by zero is attempted.
 */
HILTI_EXCEPTION(DivisionByZero, RuntimeError)

namespace integer {

namespace detail {
class SafeIntException {
public:
    static void SafeIntOnOverflow() __attribute__((noreturn)) { throw Overflow("integer overflow"); }

    static void SafeIntOnDivZero() __attribute__((noreturn)) { throw DivisionByZero("integer devision by zero"); }
};
} // namespace detail

template<typename T>
using safe = SafeInt<T, detail::SafeIntException>;

} // namespace integer
} // namespace hilti::rt

// Needs to be a top level.
template<typename O, typename T>
inline auto operator<<(O& out, const hilti::rt::integer::safe<T>& x)
    -> std::enable_if_t<std::is_base_of_v<std::ostream, O>, O>& {
    if ( std::is_same<T, int8_t>() )
        out << static_cast<int16_t>(x);
    else if ( std::is_same<T, uint8_t>() )
        out << static_cast<uint16_t>(x);
    else
        out << x.Ref();

    return out;
}
