#pragma once
#include "ADL.h"
#include "StringView.h"

#include <stddef.h> // size_t

namespace string19 {

constexpr auto ropeCount(ADL*, StringView sv) -> size_t { return sv.count; }

constexpr void ropeAppend(ADL*, char*& data, StringView sv) {
    for (auto i = 0u; i < sv.count; ++i) *data++ = sv[i];
}

} // namespace string19
