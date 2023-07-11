#pragma once

#include <optional>
#include <type_traits>
#include "mage-hand.h"

namespace mage_hand {

template<class T> class MatchResult {
    std::optional<T> _result;

  public:
    MatchResult() = default;
    template<class R>
    explicit MatchResult(R&& result): _result(std::forward<R>(result)) {}

    bool hasResult() const { return _result.has_value(); }
    operator bool() const { return hasResult(); }

    T& result() { return _result.value(); }
    T& operator*() { return result(); }
    T* operator->() { return &result(); }

    const T& result() const { return _result.value(); }
    const T& operator*() const { return result(); }
    const T* operator->() const { return &result(); }
};

template<class T> class MatchResult<T&> {
    T* _result = nullptr;

  public:
    MatchResult() = default;
    template<class R> explicit MatchResult(R& result): _result(&result) {}

    bool hasResult() const { return _result != nullptr; }
    operator bool() const { return hasResult(); }

    T& result() {
        if (!hasResult())
            throw std::bad_optional_access();
        return *_result;
    }
    T& operator*() { return result(); }
    T* operator->() { return &result(); }

    const T& result() const {
        if (!hasResult())
            throw std::bad_optional_access();
        return *_result;
    }
    const T& operator*() const { return result(); }
    const T* operator->() const { return &result(); }
};

template<> class MatchResult<void> {
    bool _result = false;

  public:
    MatchResult() = default;
    explicit MatchResult(bool result): _result(result) {}

    bool hasResult() const { return _result; }
    operator bool() const { return hasResult(); }
};

template<class T, class R> class Match {
    const T& _value;
    MatchResult<R> _result;

  public:
    explicit Match(const T& value) : _value(value) {};
    template<class RR> explicit Match(const T& value, RR&& result):
            _value(value), _result(std::forward<RR>(result)) {}

    const T& value() const { return _value; }

    bool hasResult() const { return _result.hasResult(); }
    operator bool() const { return _result; }

    decltype(auto) result() { return _result.result(); }
    decltype(auto) operator*() { return *_result; }
    decltype(auto) operator->() { return operator->(_result); }

    decltype(auto) result() const { return _result.result(); }
    decltype(auto) operator*() const { return *_result; }
    decltype(auto) operator->() const { return operator->(_result); }

    operator MatchResult<R>() { return _result; }
};

template<class T> class Match<T, void> {
    const T& _value;
    MatchResult<void> _result;

  public:
    explicit Match(const T& value) : _value(value) {};
    template<class RR> explicit Match(const T& value, RR&& result):
            _value(value), _result(std::forward<RR>(result)) {}

    const T& value() const { return _value; }

    bool hasResult() const { return _result.hasResult(); }
    operator bool() const { return _result; }

    operator MatchResult<void>() { return _result; }
};

template<class T, class R> struct HalfMatch {
    bool found;
    Match<T, R> match;

    bool hasResult() const { return match.hasResult(); }
    const T& value() const { return match.value(); }
};

struct NullType {};

template<class T> auto match(const T& value) {
    return Match<T, NullType>(value);
}

constexpr struct When : Operator {} when;
constexpr struct Then : Operator {} then;
constexpr struct Otherwise : Operator {} otherwise;

template<class T, class R, class V> struct Operator::Bar<Match<T, R>, When, V> {
    auto operator()(Match<T, R>&& match, const V& value) {
        return HalfMatch<T, R>{
                !match.hasResult() && match.value() == value, std::move(match)};
    }
};

template<class T, class R, class V>
struct Operator::Bar<HalfMatch<T, R>, When, V> {
    auto operator()(HalfMatch<T, R>&& match, const V& value) {
        return HalfMatch<T, R>{
                !match.hasResult() && (match.found || match.value() == value),
                std::move(match.match)};
    }
};

template<class T, class R, class F>
struct Operator::Bar<HalfMatch<T, R>, Then, F> {
    auto operator()(HalfMatch<T, R>&& match, F&& function) {
        using FR = std::invoke_result_t<F>;

        // std::common_type doesn't handle references correctly
        constexpr bool haveRefs =
                std::is_reference_v<R> && std::is_reference_v<FR>;
        using C = std::common_type_t<R, FR>;
        using FC = std::conditional_t<haveRefs, C&, C>;

        using M = Match<T, FC>;
        if (!match.hasResult() && match.found)
            return M(match.value(), std::forward<F>(function)());
        return M(std::move(match.match));
    }
};

template<class T, class F> struct Operator::Bar<HalfMatch<T, void>, Then, F> {
    auto operator()(HalfMatch<T, void>&& match, F&& function) {
        if (!match.hasResult() && match.found) {
            std::forward<F>(function)();
            return Match<T, void>(match.value(), true);
        }
        return Match<T, void>(std::move(match.match));
    }
};

template<class T, class F>
struct Operator::Bar<HalfMatch<T, NullType>, Then, F> {
    auto operator()(HalfMatch<T, NullType>&& match, F&& function) {
        using R = std::invoke_result_t<F>;
        return HalfMatch<T, R>{match.found, Match<T, R>(match.value())}
               |then| std::forward<F>(function);
    }
};

template<class T, class R, class F>
struct Operator::Bar<Match<T, R>, Otherwise, F> {
    decltype(auto) operator()(Match<T, R>&& match, F&& function) {
        if (!match.hasResult())
            return MatchResult<R>(std::forward<F>(function)()).result();
        return match.result();
    }
};

template<class T, class F> struct Operator::Bar<Match<T, void>, Otherwise, F> {
    void operator()(Match<T, void>&& match, F&& function) {
        if (!match.hasResult())
            std::forward<F>(function)();
    }
};

template<class T, class F>
struct Operator::Bar<Match<T, NullType>, Otherwise, F> {
    decltype(auto) operator()(Match<T, NullType>&& match, F&& function) {
        return std::forward<F>(function)();
    }
};
}  // namespace mage_hand
