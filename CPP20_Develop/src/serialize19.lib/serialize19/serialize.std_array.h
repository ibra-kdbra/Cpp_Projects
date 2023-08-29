#pragma once
#include "serialize.h"

#include <array>
#include <stddef.h> // size_t

namespace serialize19 {

// note: array has fixed size, so no size is stored
template<Archive A, class T, size_t N> void serialize(A& a, std::array<T, N>& array) {
    for (auto& value : array) {
        serialize(a, value);
    }
}

} // namespace serialize19
