#pragma once
#include "Variant.h"
#include "meta19/Type.h"

namespace variant19 {

template<class... Ts> constexpr bool operator==(const Variant<Ts...>& a, const Variant<Ts...>& b) {
    using meta19::type;
    if (a.which() != b.which()) return false;

    return a.visit([&]<class T>(const T& x) {
        const auto& y = b.as(type<T>);
        return x == y;
    });
}

template<class... Ts> constexpr bool operator!=(const Variant<Ts...>& a, const Variant<Ts...>& b) { return !(a == b); }

} // namespace variant19
