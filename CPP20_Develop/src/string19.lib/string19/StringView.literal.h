#pragma once
#include "StringView.h"

#include <stddef.h> // size_t

namespace string19 {

template<size_t N> using ConstCharArray = const char[N];

template<size_t N> constexpr auto viewLiteral(const ConstCharArray<N>& o) noexcept -> const StringView {
    return {&o[0], N - 1, ExtraZero::Present};
}

} // namespace string19
