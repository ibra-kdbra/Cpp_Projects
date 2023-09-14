#pragma once
#include "Variant.h"

#include <ostream>

namespace variant19 {

template<class Chr, class Traits, class... Ts>
auto operator<<(std::basic_ostream<Chr, Traits>& out, const Variant<Ts...>& t) -> decltype(out)& {
    out << "<";
    t.visit([&](auto& v) { out << v; });
    return out << ">";
}

} // namespace variant19
