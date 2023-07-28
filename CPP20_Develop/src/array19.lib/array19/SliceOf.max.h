#pragma once
#include "SliceOf.h"

namespace array19 {

template<class T> constexpr auto sliceMaximum(SliceOf<T> slice) {
    auto result = T{};
    for (auto v : slice)
        if (v > result) result = v;
    return result;
}

} // namespace array19
