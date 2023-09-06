#pragma once
#include "StringView.h"

namespace string19 {

constexpr bool operator==(StringView a, StringView b) noexcept {
    if (a.count != b.count) return false;
    for (auto i = 0u; i < a.count && i < b.count; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

constexpr bool operator!=(StringView a, StringView b) noexcept { return !(a == b); }

} // namespace string19
