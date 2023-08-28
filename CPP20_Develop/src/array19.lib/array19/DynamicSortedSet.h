#pragma once
#include "DynamicArrayOf.h"
#include "SliceOf.single.h"

#include <algorithm>
#include <stddef.h> // size_t

namespace array19 {

/// set of sorted of values
/// - useful for sets of ids
template<class T, class Less = std::less<>> struct DynamicSortedSet {
    [[nodiscard]] auto isEmpty() const noexcept -> bool { return m.isEmpty(); }
    [[nodiscard]] auto count() const -> size_t { return m.count(); }
    [[nodiscard]] auto begin() const -> T const* { return m.begin(); }
    [[nodiscard]] auto end() const -> T const* { return m.end(); }

    [[nodiscard]] bool has(const T& v) const {
        auto [b, e] = std::equal_range(m.begin(), m.end(), v, Less{});
        return b != e;
    }

    void add(T const& v) {
        auto [b, e] = std::equal_range(m.amendBegin(), m.amendEnd(), v, Less{});
        if (b == e) {
            m.splice(b, 0, array19::sliceOfSingle(v));
        }
    }

    void remove(T const& v) {
        auto [b, e] = std::equal_range(m.amendBegin(), m.amendEnd(), v, Less{});
        if (b != e) {
            m.remove(b, 1);
        }
    }

    void clear() { m.clear(); }

private:
    DynamicArrayOf<T> m;
};

} // namespace array19
