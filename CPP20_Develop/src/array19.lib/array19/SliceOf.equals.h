#pragma once
#include "SliceOf.h"

#include <stddef.h> // size_t

namespace array19 {

template<class T> constexpr bool operator==(const SliceOf<T>& a, const SliceOf<T>& b) {
    if (a.count() != b.count()) return false;
    for (size_t i = 0u; i < a.count() && i < b.count(); i++) {
        if (!(a[i] == b[i])) return false;
    }
    return true;
}

template<class T> constexpr bool operator!=(const SliceOf<T>& a, const SliceOf<T>& b) { return !(a == b); }

} // namespace array19
