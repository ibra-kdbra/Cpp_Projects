#pragma once
#include "Partial.h"

namespace partial19 {

template<class... Ts> bool operator==(const Partial<Ts...>& a, const Partial<Ts...>& b) noexcept {
    if (a.which() != b.which()) return false;
    return ((a.which().of(type<Ts>) ? a.of(type<Ts>) == b.of(type<Ts>) : true) && ... && true);
}

template<class... Ts> bool operator!=(const Partial<Ts...>& a, const Partial<Ts...>& b) noexcept { return !(a == b); }

} // namespace partial19
