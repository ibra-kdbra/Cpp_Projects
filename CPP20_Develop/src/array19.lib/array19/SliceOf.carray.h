#pragma once
#include "SliceOf.h"

#include <stddef.h> // size_t
namespace array19 {

template<class T, size_t N> using CArray = T[N];

template<class T, size_t N> constexpr auto sliceOfCArray(const T (&array)[N]) noexcept -> SliceOf<const T> {
    return SliceOf<const T>{array, N};
}

template<class T, size_t N> constexpr auto amendSliceOfCArray(CArray<T, N>& array) noexcept -> SliceOf<T> {
    return SliceOf<T>{array, N};
}

} // namespace array19
