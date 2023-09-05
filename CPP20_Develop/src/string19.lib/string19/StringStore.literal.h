#pragma once
#include "StringStore.h"

#include <stddef.h> // size_t

namespace string19 {

template<size_t N> constexpr auto storeLiteral(const CharArray<N>& o) noexcept -> const StringStore<N - 1> {
    auto r = StringStore<N - 1>{};
    for (auto i = 0U; i < N - 1; i++) r.data[i] = o[i];
    return r;
}

} // namespace string19
