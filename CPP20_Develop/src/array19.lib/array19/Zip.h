#pragma once
#include "adlRange.h"

namespace array19 {

/// Zip allows to iterate two containers in lockstep
///
/// precondition:
///  * both containers have the same length
///
/// usage:
///   for (auto [valueA, valueB] : Zip{a, b});
template<class A, class B> struct Zip {
    using AIt = decltype(adlBegin(std::declval<A&>()));
    using BIt = decltype(adlBegin(std::declval<B&>()));
    using ARes = decltype(*std::declval<const AIt&>());
    using BRes = decltype(*std::declval<const BIt&>());

    struct result {
        ARes a;
        BRes b;
    };

    struct iterator {
        AIt aIt;
        BIt bIt;

        constexpr auto operator*() const -> result { return {*aIt, *bIt}; }
        constexpr auto operator++() -> iterator& { return (++aIt, ++bIt, *this); }
        constexpr bool operator==(const iterator& o) const = default;

        using iterator_category = std::forward_iterator_tag;
        using value_type = result;
        using difference_type = void;
        using pointer = void*;
        using reference = result;
    };

    constexpr Zip() = default;
    constexpr explicit Zip(A& a, B& b) : a(&a), b(&b) {}

    constexpr auto begin() const -> iterator { return {adlBegin(*a), adlBegin(*b)}; }
    constexpr auto end() const -> iterator { return {adlEnd(*a), adlEnd(*b)}; }

private:
    A* a{};
    B* b{};
};

template<class A, class B> Zip(A&, B&) -> Zip<A, B>;

} // namespace array19
