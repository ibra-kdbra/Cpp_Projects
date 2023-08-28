#pragma once
#include "Partial.h"

#include <ostream>

namespace partial19 {

template<class Chr, class Traits, class... Ts>
auto operator<<(std::basic_ostream<Chr, Traits>& out, const Partial<Ts...>& t) -> decltype(out)& {
    out << "<[";
    bool first = true;
    t.visitInitialized([&](auto& v) {
        if (first)
            first = false;
        else
            out << "; ";
        out << v;
    });
    return out << "]>";
}

} // namespace partial19
