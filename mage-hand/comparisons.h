#pragma once

#include <functional>
#include "mage-hand.h"

namespace mage_hand {
template<class L, class R, class F> class ComparisonResult {
  public:
    ComparisonResult(const L& l, const R& r, bool s):
            _left(l), _right(r), _success(s) {}
    ComparisonResult(const L& l, const R& r, const F& f, bool s):
            _left(l), _right(r), _functor(f), _success(s) {}
    ComparisonResult(const L& l, const R& r, F&& f, bool s):
            _left(l), _right(r), _functor(std::move(f)), _success(s) {}
    ComparisonResult(const ComparisonResult&) = default;
    ComparisonResult& operator=(const ComparisonResult&) = default;
    ComparisonResult(ComparisonResult&&) = default;
    ComparisonResult& operator=(ComparisonResult&&) = default;

    const L& left() const { return _left; }
    const R& right() const { return _right; }
    const F& functor() const { return _functor; }
    bool success() const { return _success; }

    constexpr operator bool() const { return _success; }
    constexpr bool operator!() const { return !_success; }

    bool operator&&(bool value) const { return _success && value; }
    bool operator||(bool value) const { return _success || value; }

    template<class OL, class OR, class OF>
    bool operator&&(const ComparisonResult<OL, OR, OF>& other) {
        return bool(*this) && bool(other);
    }
    template<class OL, class OR, class OF>
    bool operator||(const ComparisonResult<OL, OR, OF>& other) {
        return bool(*this) || bool(other);
    }

    template<class T>
    ComparisonResult<L, T, F> operator&&(const T& value) const {
        return {_left, value, _functor, _success && _functor(_left, value)};
    }
    template<class T>
    ComparisonResult<L, T, F> operator||(const T& value) const {
        return {_left, value, _functor, _success || _functor(_left, value)};
    }

    template<class T>
    ComparisonResult<L, T, typename std::equal_to<>> operator==(const T& value) const {
        return {_left, value, _success && std::equal_to{}(_right, value)};
    }
    template<class T>
    ComparisonResult<L, T, typename std::not_equal_to<>> operator!=(const T& value) const {
        return {_left, value, _success && std::not_equal_to{}(_right, value)};
    }
    template<class T>
    ComparisonResult<L, T, typename std::less<>> operator<(const T& value) const {
        return {_left, value, _success && std::less{}(_right, value)};
    }
    template<class T>
    ComparisonResult<L, T, typename std::less_equal<>> operator<=(const T& value) const {
        return {_left, value, _success && std::less_equal{}(_right, value)};
    }
    template<class T>
    ComparisonResult<L, T, typename std::greater<>> operator>(const T& value) const {
        return {_left, value, _success && std::greater{}(_right, value)};
    }
    template<class T>
    ComparisonResult<L, T, typename std::greater_equal<>> operator>=(const T& value) const {
        return {_left, value, _success && std::greater_equal{}(_right, value)};
    }
  private:
    const L& _left;
    const R& _right;
    F _functor;
    bool _success;
};

template<class = void> struct IsComparisonResult : std::false_type {};
template<class L, class R, class F>
struct IsComparisonResult<ComparisonResult<L, R, F>> : std::true_type {};
template<class T>
constexpr bool isComparisonResult = IsComparisonResult<T>::value;

constexpr struct Is : Operator {} is;
constexpr struct IsNot : Operator {} is_not;
constexpr struct Less : Operator {} less, before, lt;
constexpr struct NotLess : Operator {} not_less, not_before;
constexpr struct More : Operator {} more, after, gt;
constexpr struct NotMore : Operator {} not_more, not_after;

template<class L, class R> struct Operator::Angle<L, Is, R> {
    ComparisonResult<L, R, std::equal_to<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::equal_to{}(left, right)};
    }
};

template<class L, class R> struct Operator::Angle<L, IsNot, R> {
    ComparisonResult<L, R, std::not_equal_to<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::not_equal_to{}(left, right)};
    }
};

template<class L, class R> struct Operator::Angle<L, Less, R> {
    ComparisonResult<L, R, typename std::less<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::less{}(left, right)};
    }
};

template<class L, class R> struct Operator::AngleEqual<L, Less, R> {
    ComparisonResult<L, R, typename std::less_equal<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::less_equal{}(left, right)};
    }
};

template<class L, class R> struct Operator::Angle<L, NotLess, R> {
    ComparisonResult<L, R, typename std::less<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::greater_equal{}(left, right)};
    }
};
template<class L, class R> struct Operator::AngleEqual<L, NotLess, R> {
    ComparisonResult<L, R, typename std::less_equal<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::greater{}(left, right)};
    }
};

template<class L, class R> struct Operator::Angle<L, More, R> {
    ComparisonResult<L, R, typename std::greater<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::greater{}(left, right)};
    }
};
template<class L, class R> struct Operator::AngleEqual<L, More, R> {
    ComparisonResult<L, R, typename std::greater_equal<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::greater_equal{}(left, right)};
    }
};

template<class L, class R> struct Operator::Angle<L, NotMore, R> {
    ComparisonResult<L, R, typename std::greater<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::less_equal{}(left, right)};
    }
};
template<class L, class R> struct Operator::AngleEqual<L, NotMore, R> {
    ComparisonResult<L, R, typename std::greater_equal<>>
    operator()(const L& left, const R& right) const {
        return {left, right, std::less{}(left, right)};
    }
};
}

constexpr const mage_hand::IsNot& operator!(const mage_hand::Is&) {
    return mage_hand::is_not;
}
constexpr const mage_hand::Is& operator!(const mage_hand::IsNot&) {
    return mage_hand::is;
}

constexpr const mage_hand::NotLess operator!(const mage_hand::Less&) {
    return mage_hand::not_less;
}
constexpr const mage_hand::Less operator!(const mage_hand::NotLess&) {
    return mage_hand::less;
}

constexpr const mage_hand::NotMore operator!(const mage_hand::More&) {
    return mage_hand::not_more;
}
constexpr const mage_hand::More operator!(const mage_hand::NotMore&) {
    return mage_hand::more;
}
