#pragma once
#include <algorithm>
#include "comparisons.h"
#include "math.h"

namespace mage_hand {
constexpr struct Near : Operator {
    constexpr Near() = default;
    constexpr Near(double e): epsilon(e) {}
    const double epsilon = 0.001;
} near;
constexpr struct Far : Operator {
    constexpr Far() = default;
    constexpr Far(double e): epsilon(e) {}
    const double epsilon = 0.001;
} far;
}  // namespace mage_hand

constexpr mage_hand::Near operator~(const mage_hand::Is&) {
    return {};
}
constexpr mage_hand::Far operator~(const mage_hand::IsNot&) {
    return {};
}
constexpr mage_hand::Far operator!(const mage_hand::Near& near) {
    return { near.epsilon };
}
constexpr mage_hand::Near operator!(const mage_hand::Far& far) {
    return { far.epsilon };
}

namespace mage_hand {
template<class L, class R>
struct Operator::Angle<L, Overload<Near>::If<areArithmetic<L, R>>, R> {
    Angle(const Near& near) : epsilon(near.epsilon) {}
    ComparisonResult<L, R, Near> operator()(const L& left, const R& right) const {
        if (right == 0.0) {
            return {left, right, Near(epsilon), -epsilon <lt>= left <lt>= epsilon};
        } else {
            auto e = 1.0 + epsilon;
            auto [min, max] = std::minmax(right / e, right * e);
            return {left, right, Near(epsilon), min <lt>= left <lt>= max};
        }
    }
    const double epsilon;
};

template<class L, class R>
struct Operator::Angle<L, Overload<Far>::If<areArithmetic<L, R>>, R> {
    Angle(const Far& far) : epsilon(far.epsilon) {}
    ComparisonResult<L, R, Far> operator()(const L& left, const R& right) const {
        return { left, right, Far(epsilon), !(left <Near(epsilon)> right) };
    }
    const double epsilon;
};
}  // namespace mage_hand
