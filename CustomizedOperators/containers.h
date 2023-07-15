#pragma once
#include <algorithm>
#include <type_traits>
#include <utility>
#include "comparisons.h"
#include "mage-hand.h"
#include "type-traits.h"

namespace mage_hand {
constexpr struct In : Operator {} in;
constexpr struct NotIn : Operator {} not_in;
}

constexpr const mage_hand::NotIn& operator!(const mage_hand::In&) {
    return mage_hand::not_in;
}
constexpr const mage_hand::In& operator!(const mage_hand::NotIn&) {
    return mage_hand::in;
}

namespace mage_hand {
template<class L, class W, class R>
using AngleResult = ComparisonResult<L, R, OperatorAngle<W>>;

template<class L, class R> struct Operator::Angle<L, NotIn, R> {
    template<class LL, class RR>
    AngleResult<L, NotIn, R>
    operator()(LL&& left, RR&& right) const {
        return {std::forward<LL>(left),
                std::forward<RR>(right),
                !(std::forward<LL>(left) <in> std::forward<RR>(right))};
    }
};

template<class L, class R, class F, class C>
struct Operator::Angle<ComparisonResult<L, R, F>, In, C> {
    AngleResult<L, In, C>
    operator()(const ComparisonResult<L, R, F>& result, const C& container) const {
        return {result.left(), container, result.success() && result.right() <in> container};
    }
};

template<class T, class L, class R, class F>
struct Operator::Angle<T, In, ComparisonResult<L, R, F>> {
    AngleResult<T, In, R>
    operator()(const T& value, const ComparisonResult<L, R, F>& result) const {
        return {value, result.right(), result.success() && value <in> result.left()};
    }
};

template<class L, class R, class F, class C>
struct Operator::Angle<ComparisonResult<L, R, F>, NotIn, C> {
    AngleResult<L, NotIn, C>
    operator()(const ComparisonResult<L, R, F>& result, const C& container) const {
        return {result.left(), container, result.success() && result.right() <!in> container};
    }
};

template<class T, class L, class R, class F>
struct Operator::Angle<T, NotIn, ComparisonResult<L, R, F>> {
    AngleResult<T, NotIn, R>
    operator()(const T& value, const ComparisonResult<L, R, F>& result) const {
        return {value, result.right(), result.success() && value <!in> result.left()};
    }
};

template<class C, class V, class = void> struct HasStringFind : std::false_type {};
template<class C, class V>
struct HasStringFind<C, V, std::void_t<decltype(std::declval<C>().find(std::declval<V>()) != C::npos)>>
    : std::true_type {};
template<class C, class V>
constexpr bool hasStringFind =
        HasStringFind<C, V>::value && !isComparisonResult<V>;

template<class C, class T>
struct Operator::Angle<T, Overload<In>::If<hasStringFind<C, T>>, C> {
    AngleResult<T, In, C> operator()(const T& value, const C& container) const {
        return {value, container, container.find(value) != C::npos};
    }
};

template<class C, class V, class = void> struct HasFind : std::false_type {};
template<class C, class V>
struct HasFind<C, V, std::void_t<decltype(std::declval<C>().find(std::declval<V>()) != std::end(std::declval<C>()))>>
    : std::true_type {};
template<class C, class V>
constexpr bool hasFind = HasFind<C, V>::value && !isComparisonResult<V>;

template<class C, class T>
struct Operator::Angle<T, Overload<In>::If<!hasStringFind<C, T> && hasFind<C, T>>, C> {
    AngleResult<T, In, C> operator()(const T& value, const C& container) const {
        return {value, container, container.find(value) != container.end()};
    }
};

template<class T, size_t I> struct Operator::Angle<T, In, T[I]> {
    using A = T[I];
    AngleResult<T, In, A> operator()(const T& value, const A& array) const {
        bool found = std::find(std::begin(array), std::end(array), value)
                     != std::end(array);
        return {value, array, found};
    }
};

template<class C, class V, class = void> struct IsFindable : std::false_type {};
template<class C, class V>
struct IsFindable<C, V, std::void_t<decltype(std::find(std::begin(std::declval<C>()), std::end(std::declval<C>()), std::declval<V>()) != std::end(std::declval<C>()))>>
    : std::true_type {};
template<class C, class V>
constexpr bool isFindable = IsFindable<C, V>::value && !isComparisonResult<V>;

template<class C, class T>
struct Operator::Angle<T, Overload<In>::If<!hasStringFind<C, T> && !hasFind<C, T> && isFindable<C, T>>, C> {
    AngleResult<T, In, C> operator()(const T& value, const C& container) const {
        bool found = std::find(std::begin(container), std::end(container), value)
                     != std::end(container);
        return {value, container, found};
    }
};
}
