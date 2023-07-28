#pragma once
#include "adlRange.h"

#include <stddef.h> // size_t

namespace array19 {

/// Iterate a container with indices
/// usage:
///    for(auto [value, index] : WithIndex{container});
template<class C> struct WithIndex {
    using It = decltype(adlBegin(*static_cast<C*>(nullptr)));
    using Res = decltype(**static_cast<It*>(nullptr));

    // not using std::pair to avoid reference hassles
    struct result {
        Res value;
        size_t index;
    };

    // minimal iterator that is just enough to run ranged for loop
    struct iterator {
        It it;
        size_t index{};

        [[nodiscard]] constexpr auto operator*() const -> result { return {*it, index}; }
        [[nodiscard]] constexpr bool operator!=(const It& o) const { return it != o; }
        constexpr auto operator++() -> iterator& { return (++it, ++index, *this); }
    };

    constexpr WithIndex(C& c) noexcept : c(c) {}

    [[nodiscard]] constexpr auto begin() -> iterator { return iterator{adlBegin(c), {}}; }
    [[nodiscard]] constexpr auto end() -> It { return adlEnd(c); }

private:
    C& c;
};

template<class C> WithIndex(C&) -> WithIndex<C>;

} // namespace array19
