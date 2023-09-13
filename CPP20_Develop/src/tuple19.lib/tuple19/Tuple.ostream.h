#pragma once
#include "Tuple.h"

#include <ostream>

namespace tuple19 {

template<class Chr, class Traits, class... Ts>
auto operator<<(std::basic_ostream<Chr, Traits>& out, const Tuple<Ts...>& t) -> decltype(out)& {
    out << "[";
    t.visitAllWithIndex([&](auto i, auto& v) {
        if (i != 0) out << "; ";
        out << v;
    });
    return out << "]";
}

} // namespace tuple19
