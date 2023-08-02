#pragma once
#include "array19/Array.h"
#include "string19/StringView.h"

#include <stddef.h> // size_t

namespace enum19::details {

using array19::Array;
using string19::ExtraZero;
using string19::StringView;

constexpr bool isCppIdentifierChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

template<size_t N> constexpr auto extractEnumMemberNames(StringView body) -> Array<StringView, N> {
    auto result = Array<StringView, N>{};
    auto i = 0;
    auto p = body.data;
    auto e = body.data + body.count;

    while (p != e) {
        while (!isCppIdentifierChar(*p)) {
            ++p;
            if (p == e) return result; // reached end while looking for start
        }
        auto identBegin = p;
        while (p != e && isCppIdentifierChar(*p)) ++p;
        result.amend()[i++] = StringView{identBegin, static_cast<unsigned>(p - identBegin), ExtraZero::Missing};

        auto nested = 0;
        auto quoted = char{};
        auto lastChr = char{};
        while (p != e) {
            if (quoted) {
                if (lastChr != '\\' && *p == quoted) {
                    quoted = false;
                }
            }
            else if (nested == 0 && *p == ',') {
                ++p; // skip ','
                break; // next enum begins
            }
            switch (*p) {
            case '"':
            case '\'': quoted = *p; break;
            case '(':
            case '{': nested++; break;
            case ')':
            case '}': nested--; break;

            default: break;
            }
            lastChr = *p++;
        }
    }
    return result;
}

} // namespace enum19::details
