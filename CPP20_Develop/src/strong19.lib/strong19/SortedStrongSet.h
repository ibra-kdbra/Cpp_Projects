#pragma once
#include "Strong.less.h"
#include "array19/DynamicArrayOf.h"
#include "array19/SliceOf.single.h"

#include <algorithm>
#include <stddef.h> // size_t

namespace strong19 {

/// set of sorted strong values
/// - useful for sets of ids
template<class T> struct SortedStrongSet {
    auto count() const -> size_t { return m.count(); }
    auto begin() const -> const T* { return m.begin(); }
    auto end() const -> const T* { return m.end(); }

    bool has(T v) const {
        auto [b, e] = std::equal_range(m.begin(), m.end(), v, StrongLess<T>{});
        return b != e;
    }

    void add(T v) {
        auto [b, e] = std::equal_range(m.amendBegin(), m.amendEnd(), v, StrongLess<T>{});
        if (b == e) {
            m.splice(b, 0, array19::sliceOfSingle(v));
        }
    }

    void remove(T v) {
        auto [b, e] = std::equal_range(m.amendBegin(), m.amendEnd(), v, StrongLess<T>{});
        if (b != e) {
            m.remove(b, 1);
        }
    }

    void clear() { m.clear(); }

private:
    array19::DynamicArrayOf<T> m;
};

} // namespace strong19
