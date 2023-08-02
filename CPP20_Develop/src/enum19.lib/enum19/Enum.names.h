#pragma once
#include "Enum.h"

#include <string19/Rope.Decimal.h>
#include <string19/Rope.StringView.h>
#include <string19/Rope.char.h>
#include <string19/Rope.store.h>
#include <string19/StringView.store.h>

namespace enum19 {

using string19::decimal;
using string19::Rope;
using string19::viewLiteral;
using string19::viewStore;

/// StringStore with "[unknown(xx)]" where xx is decimal representation of v
template<auto v>
constexpr auto unknown_of = [] {
    constexpr auto rope = Rope{viewLiteral("[unknown("), decimal<v>, viewLiteral(")]")};
    constexpr auto N = ropeCount(rope);
    return ropeStore<N>(rope);
}();

/// true if v is a known member of the enum
template<auto v> requires(std::is_enum_v<decltype(v)>) constexpr auto is_member = [] {
    using T = decltype(v);
    for (auto& m : meta_enum_for<T>.members) {
        if (m.value == v) return true;
    }
    return false;
}();

/// StringView of member name or "[unknown]" if invalid
template<class T> requires(std::is_enum_v<T>) constexpr auto valueName(T v) {
    for (auto& m : meta_enum_for<T>.members) {
        if (m.value == v) return m.name;
    }
    return viewLiteral("[unknown]");
}

template<auto v> constexpr auto value_name = valueName(v);

/// StringStore of "Enum::"+value_name<v>
template<auto v> requires(std::is_enum_v<decltype(v)>) constexpr auto type_value_name = [] {
    using T = decltype(v);
    constexpr auto rope = Rope{meta_enum_for<T>.name, viewLiteral("::"), value_name<v>};
    constexpr auto N = ropeCount(rope);
    return ropeStore<N>(rope);
}();

/// StringStore of value_name<v> + " (xx)" where xx is the decimal representation of v
template<auto v> requires(std::is_enum_v<decltype(v)>) constexpr auto debug_value_name = [] {
    using T = decltype(v);
    constexpr auto underlying = static_cast<std::underlying_type_t<T>>(v);
    if constexpr (is_member<v>) {
        constexpr auto rope = Rope{value_name<v>, viewLiteral(" ("), decimal<underlying>, ')'};
        constexpr auto N = ropeCount(rope);
        return ropeStore<N>(rope);
    }
    else {
        return unknown_of<underlying>;
    }
}();

} // namespace enum19
