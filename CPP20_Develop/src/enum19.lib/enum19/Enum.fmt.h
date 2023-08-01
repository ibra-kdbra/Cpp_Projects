#pragma once
#include "Enum.names.h"
#include "string19/StringView.fmt.h"

/// noto: for this header you need fmt library (not included as library dependency)
#include <fmt/format.h>

/// adds enum support for fmt
template<enum19::HasMetaEnum T, class Char> struct fmt::formatter<T, Char> {
    constexpr auto parse(fmt::basic_format_parse_context<Char>& ctx) { return ctx.begin(); }

    template<typename FormatCtx> auto format(const T& v, FormatCtx& ctx) {
        auto underlying = static_cast<std::underlying_type_t<T>>(v);
        return fmt::format_to(ctx.out(), "{} ({})", enum19::valueName(v), underlying);
    }
};
