#pragma once
#include "Strong.h"
#include "string19/StringView.fmt.h"

/// noto: for this header you need fmt library (not included as library dependency)
#include <fmt/format.h>

/// adds strong support for fmt
template<class T, class Char> requires(strong19::is_strong<T>) struct fmt::formatter<T, Char> {
    constexpr auto parse(fmt::basic_format_parse_context<Char>& ctx) { return ctx.begin(); }

    template<typename FormatContext> auto format(const T& v, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{} [{}]", v.v, strong19::strong_name<T>);
    }
};
