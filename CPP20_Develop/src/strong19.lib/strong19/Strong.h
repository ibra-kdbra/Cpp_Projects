#pragma once
#include "ADL.h"
#include "Macro.h"
#include "meta19/TypePack.h"
#include "string19/StringView.literal.h"

#ifndef Q_MOC_RUN
#    include <compare>
#endif
#include <type_traits>

namespace strong19 {

/// true if T was defined as a strong type
template<class T> concept is_strong = isStrong(nullptr_to<ADL>, nullptr_to<T>);

/// view to the name of the strong type T (is_strong<T> == true)
template<class T> constexpr inline auto strong_name = strongName(nullptr_to<T>);

/// private base class for a strong type
/// note: required or constexpr would be an error for non-constexpr types (like std::string)
/// note: operator== is automatically removed when V is not equality comparable
template<class V> struct Weak {
    static_assert(!is_strong<V>);
    V v{};
    constexpr Weak() = default;
    template<class... Ts> requires(sizeof...(Ts) > 0 && std::is_constructible_v<V, Ts...>)
    constexpr explicit Weak(Ts&&... ts) : v((Ts &&) ts...) {}
    bool operator==(const Weak&) const = default;
};

#ifndef DECLARE_STRONG_EXTRAS
#    define DECLARE_STRONG_EXTRAS(NAME)
#endif
#ifndef DEFINE_STRONG_EXTRAS
#    define DEFINE_STRONG_EXTRAS(NAME)
#endif

/// Forward declares a strong value type with name @param NAME that stores value of type @param VALUE and optional tags
#define DECLARE_STRONG(NAME, VALUE, ...)                                                                               \
    struct NAME;                                                                                                       \
    constexpr inline auto isStrong(strong19::ADL*, NAME*)->bool { return true; }                                       \
    auto strongValueType(NAME*)->STRONG19_REMOVEPAREN(VALUE);                                                          \
    auto strongTags(NAME*)->meta19::TypePack<__VA_ARGS__>;                                                             \
    constexpr inline auto strongName(NAME*)->string19::StringView { return string19::viewLiteral(#NAME); }             \
    DECLARE_STRONG_EXTRAS(NAME)                                                                                        \
    struct NAME

/// Without this supression clang would trigger a warning
#if defined(__clang__)
#    define STRONG19_IGNORE_DEFAULTED_FUNCTION_DELETED                                                                 \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#    define STRONG19_RESTORE_DEFAULTED_FUNCTION_DELETED _Pragma("clang diagnostic pop")
#else
#    define STRONG19_IGNORE_DEFAULTED_FUNCTION_DELETED
#    define STRONG19_RESTORE_DEFAULTED_FUNCTION_DELETED
#endif

/// Defines a strong value type with name @param NAME that stores value of type @param VALUE and optional tags
#define DEFINE_STRONG(NAME, VALUE, ...)                                                                                \
    DECLARE_STRONG(NAME, VALUE, ##__VA_ARGS__) : private strong19::Weak<STRONG19_REMOVEPAREN(VALUE)> {                 \
        using Weak::Weak;                                                                                              \
        using Weak::v;                                                                                                 \
        STRONG19_IGNORE_DEFAULTED_FUNCTION_DELETED                                                                     \
        bool operator==(const NAME&) const = default;                                                                  \
        STRONG19_RESTORE_DEFAULTED_FUNCTION_DELETED                                                                    \
    };                                                                                                                 \
    DEFINE_STRONG_EXTRAS(NAME)                                                                                         \
    struct NAME

/// Fallback for any type that does not have isStrong overload
/// note: This overload is found through argument dependent lookup of strong19::ADL
constexpr inline auto isStrong(ADL*, const void*) -> bool { return false; }

/// stored type of the strong type T (is_strong<T> == true)
template<class T> using StrongValueType = decltype(strongValueType(nullptr_to<T>));

/// typepack with all tags of the strong type T (is_strong<T> == true)
template<class T> using StrongTags = decltype(strongTags(nullptr_to<T>));

} // namespace strong19
