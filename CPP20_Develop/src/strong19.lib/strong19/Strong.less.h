#pragma once
#include "Strong.h"

namespace strong19 {

template<class T> requires(is_strong<T>) struct StrongLess {
    constexpr bool operator()(const T& l, const T& r) const { return l.v < r.v; }
};

} // namespace strong19
