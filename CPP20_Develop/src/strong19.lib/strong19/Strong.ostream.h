#pragma once
#include "Strong.h"
#include "string19/StringView.ostream.h"

#include <ostream>

namespace strong19 {

template<class T>
concept Ostreamable = requires(T v, std::ostream& out) {
                          { out << v.v };
                      };

template<class S> auto strongOstream(std::ostream& out, const S& s) -> std::ostream& {
    if constexpr (Ostreamable<S>) {
        return out << strong_name<S> << '{' << s.v << '}';
    }
    else {
        return out << strong_name<S> << "{<unprintable>}";
    }
}

/// defines an ostream operator for given strong type (also prints name of the strong type)
#define DEFINE_STRONG_OSTREAM(NAME)                                                                                    \
    inline auto operator<<(std::ostream& out, const NAME& s)->std::ostream& { return strong19::strongOstream(out, s); }

} // namespace strong19
