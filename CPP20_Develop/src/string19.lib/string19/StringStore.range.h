#pragma once
#include "StringStore.h"

#include <stddef.h> // size_t

namespace string19 {

template<size_t N> constexpr auto begin(const StringStore<N>& s) noexcept -> const char* { return s.data; }
template<size_t N> constexpr auto end(const StringStore<N>& s) noexcept -> const char* { return s.data + s.count; }

} // namespace string19
