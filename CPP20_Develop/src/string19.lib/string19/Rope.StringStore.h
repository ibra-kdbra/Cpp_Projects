#pragma once
#include "ADL.h"
#include "StringStore.h"

#include <stddef.h> // size_t

namespace string19 {

template<size_t N> constexpr auto ropeCount(ADL*, const StringStore<N>&) -> size_t { return N; }

template<size_t N> constexpr void ropeAppend(ADL*, char*& data, const StringStore<N>& s) {
    for (auto i = 0u; i < N; ++i) *data++ = s[i];
}

} // namespace string19
