#pragma once
#include "AllocatedArrayOf.h"
#include "Zip.h"

namespace array19 {

template<class T> bool operator==(const AllocatedArrayOf<T>& a, const AllocatedArrayOf<T>& b) noexcept {
    if (a.count() != b.count()) return false;
    for (auto [av, bv] : Zip(a, b)) {
        if (!(av == bv)) return false;
    }
    return true;
}

template<class T> bool operator!=(const AllocatedArrayOf<T>& a, const AllocatedArrayOf<T>& b) noexcept {
    return !(a == b);
}

} // namespace array19
