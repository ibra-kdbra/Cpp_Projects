#pragma once
#include <type_traits>
#include "type-traits.h"

namespace mage_hand {

struct Operator {
    template<class L, class W, class R> struct Star {};         // a *x* b
    template<class L, class W, class R> struct Slash {};        // a /x/ b
    template<class L, class W, class R> struct Percent {};      // a %x% b
    template<class L, class W, class R> struct Plus {};         // a +x+ b
    template<class L, class W, class R> struct Minus {};        // a -x- b
    template<class L, class W, class R> struct LeftArrow {};    // a <x- b
    template<class L, class W, class R> struct RightArrow {};   // a -x> b
    template<class L, class W, class R> struct DoubleAngle {};  // a <<x>> b
    template<class L, class W, class R> struct Angle {};        // a <x> b
    template<class L, class W, class R> struct AngleEqual {};   // a <x>= b
    template<class L, class W, class R> struct Ampersand {};    // a &x& b
    template<class L, class W, class R> struct Caret {};        // a ^x^ b
    template<class L, class W, class R> struct Bar {};          // a |x| b
};

#define MH_DEFINE_DEDUCER(name)                                                 \
    template<class W> class Operator##name {                                    \
        template<class L, class R>                                              \
        using F = typename W::template name<RemoveCvRef<L>, W, RemoveCvRef<R>>; \
                                                                                \
      public:                                                                   \
        template<class L, class R>                                              \
        static constexpr bool isWordAware =                                     \
                std::is_constructible_v<F<L, R>, const W&>;                     \
                                                                                \
        template<class L, class R,                                              \
                If<std::is_default_constructible_v<F<L, R>>> = true>            \
        decltype(auto) operator()(L&& left, R&& right) {                        \
            F<L, R> f;                                                          \
            return f(std::forward<L>(left), std::forward<R>(right));            \
        }                                                                       \
        template<class L, class R,                                              \
                If<std::is_default_constructible_v<F<L, R>>> = true>            \
        decltype(auto) operator()(L&& left, R&& right) const {                  \
            F<L, R> f;                                                          \
            return f(std::forward<L>(left), std::forward<R>(right));            \
        }                                                                       \
        template<class L, class R, If<isWordAware<L, R>> = true>                \
        decltype(auto) operator()(L&& left, const W& word, R&& right) {         \
            F<L, R> f{word};                                                    \
            return f(std::forward<L>(left), std::forward<R>(right));            \
        }                                                                       \
        template<class L, class R, If<isWordAware<L, R>> = true>                \
        decltype(auto) operator()(L&& left, const W& word, R&& right) const {   \
            F<L, R> f{word};                                                    \
            return f(std::forward<L>(left), std::forward<R>(right));            \
        }                                                                       \
    };                                                                          \
    template<class L, class W, class R>                                         \
    constexpr bool isWordAware##name =                                          \
            Operator##name<W>::template isWordAware<L, R>

MH_DEFINE_DEDUCER(Star);
MH_DEFINE_DEDUCER(Slash);
MH_DEFINE_DEDUCER(Percent);
MH_DEFINE_DEDUCER(Plus);
MH_DEFINE_DEDUCER(Minus);
MH_DEFINE_DEDUCER(LeftArrow);
MH_DEFINE_DEDUCER(RightArrow);
MH_DEFINE_DEDUCER(DoubleAngle);
MH_DEFINE_DEDUCER(Angle);
MH_DEFINE_DEDUCER(AngleEqual);
MH_DEFINE_DEDUCER(Ampersand);
MH_DEFINE_DEDUCER(Caret);
MH_DEFINE_DEDUCER(Bar);

template<class T> constexpr bool isOperator = std::is_base_of_v<Operator, T>;

template<class T, class O, class W> struct Value {
    T&& value;
    const W& word;
};

template<class T, class W> using StarValue = Value<T, struct StarTag, W>;
template<class T, class W> using SlashValue = Value<T, struct SlashTag, W>;
template<class T, class W> using PercentValue = Value<T, struct PercentTag, W>;
template<class T, class W> using PlusValue = Value<T, struct PlusTag, W>;
template<class T, class W> using MinusValue = Value<T, struct MinusTag, W>;
template<class T, class W> using LeftDoubleAngleValue = Value<T, struct LeftDoubleAngleTag, W>;
template<class T, class W> using LeftAngleValue = Value<T, struct LeftAngleTag, W>;
template<class T, class W> using AmpersandValue = Value<T, struct AmpersandTag, W>;
template<class T, class W> using CaretValue = Value<T, struct CaretTag, W>;
template<class T, class W> using BarValue = Value<T, struct BarTag, W>;
}  // namespace mage_hand

#define MH_OVERLOAD_LEFT_PRIMARY(name, op)                                     \
    template<class T, class W, mage_hand::If<mage_hand::isOperator<W>> = true> \
    decltype(auto) operator op(T&& value, const W& word) {                     \
        return mage_hand::name##Value<T, W>{std::forward<T>(value), word};     \
    }

#define MH_OVERLOAD_RIGHT_PRIMARY(name, op)                                    \
    template<class T, class W, mage_hand::If<mage_hand::isOperator<W>> = true> \
    decltype(auto) operator op(const W& word, T&& value) {                     \
        return mage_hand::name##Value<T, W>{std::forward<T>(value), word};     \
    }

#define MH_OVERLOAD_LEFT_SECONDARY(name, op, rhs)                               \
    template<class L, class W, class R,                                         \
            mage_hand::If<!mage_hand::isWordAware##name<L, W, R>> = true>       \
    decltype(auto) operator op(R&& right, mage_hand::rhs##Value<L, W>&& left) { \
        using F = typename mage_hand::Operator##name<W>;                        \
        return F{}(std::forward<L>(left.value), std::forward<R>(right));        \
    }                                                                           \
    template<class L, class W, class R,                                         \
            mage_hand::If<mage_hand::isWordAware##name<L, W, R>> = true>        \
    decltype(auto) operator op(R&& right, mage_hand::rhs##Value<L, W>&& left) { \
        using F = typename mage_hand::Operator##name<W>;                        \
        return F{}(std::forward<L>(left.value), left.word,                      \
                std::forward<R>(right));                                        \
    }

#define MH_OVERLOAD_RIGHT_SECONDARY(name, op, lhs)                              \
    template<class L, class W, class R,                                         \
            mage_hand::If<!mage_hand::isWordAware##name<L, W, R>> = true>       \
    decltype(auto) operator op(mage_hand::lhs##Value<L, W>&& left, R&& right) { \
        using F = typename mage_hand::Operator##name<W>;                        \
        return F{}(std::forward<L>(left.value), std::forward<R>(right));        \
    }                                                                           \
    template<class L, class W, class R,                                         \
            mage_hand::If<mage_hand::isWordAware##name<L, W, R>> = true>        \
    decltype(auto) operator op(mage_hand::lhs##Value<L, W>&& left, R&& right) { \
        using F = typename mage_hand::Operator##name<W>;                        \
        return F{}(std::forward<L>(left.value), left.word,                      \
                std::forward<R>(right));                                        \
    }

#define MH_OVERLOAD_SIMPLE_OPERATOR(name, op) \
    MH_OVERLOAD_LEFT_PRIMARY(name, op)        \
    MH_OVERLOAD_RIGHT_SECONDARY(name, op, name)

MH_OVERLOAD_SIMPLE_OPERATOR(Star, *)
MH_OVERLOAD_SIMPLE_OPERATOR(Slash, /)
MH_OVERLOAD_SIMPLE_OPERATOR(Percent, %)
MH_OVERLOAD_SIMPLE_OPERATOR(Plus, +)
MH_OVERLOAD_SIMPLE_OPERATOR(Ampersand, &)
MH_OVERLOAD_SIMPLE_OPERATOR(Caret, ^)
MH_OVERLOAD_SIMPLE_OPERATOR(Bar, |)

MH_OVERLOAD_LEFT_PRIMARY(Minus, -)
MH_OVERLOAD_RIGHT_SECONDARY(Minus, -, Minus)
MH_OVERLOAD_RIGHT_SECONDARY(RightArrow, >, Minus)

MH_OVERLOAD_LEFT_SECONDARY(LeftArrow, <, Minus)
MH_OVERLOAD_RIGHT_PRIMARY(Minus, -)

MH_OVERLOAD_LEFT_PRIMARY(LeftAngle, <)
MH_OVERLOAD_RIGHT_SECONDARY(Angle, >, LeftAngle)
MH_OVERLOAD_RIGHT_SECONDARY(AngleEqual, >=, LeftAngle)

MH_OVERLOAD_LEFT_PRIMARY(LeftDoubleAngle, <<)
MH_OVERLOAD_RIGHT_SECONDARY(DoubleAngle, >>, LeftDoubleAngle)
