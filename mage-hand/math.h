#pragma once
#include <stdexcept>
#include <type_traits>
#include <cmath>
#include "mage-hand.h"

namespace mage_hand {
template<class L, class R>
constexpr bool areIntegral = std::is_integral_v<L> and std::is_integral_v<R>;

template<class L, class R, If<areIntegral<L, R>> = true>
constexpr L pow(L n, R e) {
    if (n == 0 && e <= 0)
        throw std::domain_error("0^x is undefined if x <= 0");
    if (e < 0)
        return n == 1 ? 1 : 0;
    L r = 1;
    while (e > 0) {
        r *= e % 2 ? n : 1;
        n *= n;
        e /= 2;
    }
    return r;
}

constexpr struct Mod : Operator {} mod;
constexpr struct Xx : Operator {} xx;

template<class L, class R>
struct Operator::Slash<L, Overload<Mod>::If<areIntegral<L, R>>, R> {
    auto operator()(L lhs, R rhs) {
        if (rhs <= 0)
            throw std::domain_error("n mod m undefined if m <= 0");
        return (lhs % rhs + rhs) % rhs;
    }
};

template<class L, class R>
constexpr bool areArithmetic = std::is_arithmetic_v<L> and std::is_arithmetic_v<R>;

template<class L, class R>
constexpr bool anyFp = std::is_floating_point_v<L> or std::is_floating_point_v<R>;

template<class L, class R>
struct Operator::Slash<L, Overload<Mod>::If<areArithmetic<L, R> && anyFp<L, R>>, R> {
    auto operator()(L lhs, R rhs) {
        return std::fmod(std::fmod(lhs, rhs) + rhs, rhs);
    }
};

template<class L, class R>
struct Operator::Slash<L, Overload<Xx>::If<areIntegral<L, R>>, R> {
    L operator()(L lhs, R rhs) { return pow(lhs, rhs); }
};

template<class L, class R>
struct Operator::Slash<L, Overload<Xx>::If<areArithmetic<L, R> && anyFp<L, R>>, R> {
    L operator()(L lhs, R rhs) { return std::pow(lhs, rhs); }
};

template<class T> struct Div : Operator {};
constexpr Div<float> fdiv;
constexpr Div<double> div;
constexpr Div<long double> ldiv;

template<class L, class R>
struct Operator::Slash<L, typename Overload<Div<float>>::If<areArithmetic<L, R>>, R> {
    float operator()(L lhs, R rhs) {
        return static_cast<float>(lhs) / static_cast<float>(rhs);
    }
};

template<class L, class R>
struct Operator::Slash<L, typename Overload<Div<double>>::If<areArithmetic<L, R>>, R> {
    double operator()(L lhs, R rhs) {
        return static_cast<double>(lhs) / static_cast<double>(rhs);
    }
};

template<class L, class R>
struct Operator::Slash<L, typename Overload<Div<long double>>::If<areArithmetic<L, R>>, R> {
    long double operator()(L lhs, R rhs) {
        return static_cast<long double>(lhs) / static_cast<long double>(rhs);
    }
};
}
