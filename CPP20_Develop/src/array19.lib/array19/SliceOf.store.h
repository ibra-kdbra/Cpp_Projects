#pragma once
#include "Array.h"
#include "SliceOf.h"

#include <stddef.h> // size_t
#include <utility>

namespace array19 {

/// note:
///     You have to know the size at compile time to create an Array!
/// usage:
///     constexpr auto slice = SliceOf...;
///     constexpr auto array = storeSlice<slice.count()>(slice);
template<size_t N, class T> constexpr auto storeSlice(const SliceOf<T>& slice) noexcept -> Array<T, N> {
    return [&]<size_t... Is>(std::index_sequence<Is...>*)->Array<T, N> { return {slice[Is]...}; }
    (static_cast<std::make_index_sequence<N>*>(nullptr));
}

} // namespace array19
