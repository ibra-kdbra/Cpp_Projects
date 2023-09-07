#pragma once
#include "StringStore.h"
#include "StringView.h"

#include <stddef.h> // size_t

namespace string19 {

template<size_t N> constexpr auto viewStore(const StringStore<N>& s) -> StringView {
    return {s.data, static_cast<unsigned>(s.count), ExtraZero::Present};
}

} // namespace string19
