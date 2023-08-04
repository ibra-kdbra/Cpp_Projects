#pragma once
#include "Index.h"

#include <stddef.h> // size_t

namespace meta19 {

template<class C> inline constexpr auto index_value = nullptr;
template<size_t V> inline constexpr auto index_value<Index<V>> = V;

} // namespace meta19
