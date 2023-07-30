#include "CoEnumerator.h"

namespace coro19 {

/// empty sentinal
struct CoEnumerator_End final {};
template<class V> constexpr auto end(const CoEnumerator<V>&) -> CoEnumerator_End { return {}; }

template<class V> struct CoEnumerator_Iterator final {
    using iterator_category = std::input_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = V;
    using reference = V const&;
    using pointer = V const*;

    CoEnumerator<V>& gen;

    auto operator*() const -> const V& { return *gen; }
    auto operator++() -> CoEnumerator_Iterator& { return ++gen, *this; }
    bool operator==(CoEnumerator_End) const { return !gen; }
    bool operator!=(CoEnumerator_End) const { return !!gen; }
};
template<class V> auto begin(CoEnumerator<V>& gen) { return CoEnumerator_Iterator<V>{++gen}; }

} // namespace coro19
