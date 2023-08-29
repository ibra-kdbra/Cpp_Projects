#pragma once
#include "serialize.h"
#include "strong19/Strong.h" // requires strong19

namespace serialize19 {

template<Archive A, class T> requires(strong19::is_strong<T>) void serialize(A& a, T& strong) {
    serialize(a, strong.v);
}

} // namespace serialize19
