#pragma once
#include "ADL.h"
#include "meta19/nullptr_to.h"

#include <stddef.h> // size_t

namespace string19 {

using meta19::nullptr_to;

template<int V> struct Decimal;
template<int V> constexpr auto& decimal = nullptr_to<Decimal<V>>;

template<int V> constexpr auto ropeCount(ADL*, Decimal<V>*) -> size_t {
    auto c = 1u;
    auto N = static_cast<unsigned>((V < 0) ? (c++, -V) : V);
    for (auto n = 10u; n <= N; n *= 10) c++;
    return c;
}

template<int V> constexpr void ropeAppend(ADL*, char*& data, Decimal<V>*) {
    auto c = static_cast<int>(ropeCount(adl, decimal<V>));
    auto n = V < 0 ? -V : V;
    for (int o = c - 1; o >= 0; o--, n /= 10) data[o] = n % 10 + '0';
    if (V < 0) data[0] = '-';
    data += c;
}

} // namespace string19
