#pragma once
#include <stddef.h> // size_t

namespace string19 {

enum class ExtraZero : unsigned {
    Missing,
    Present,
};

struct StringView {
    const char* data{};
    unsigned count{};
    ExtraZero extraZero{};

    constexpr auto operator[](size_t i) const noexcept -> char { return data[i]; }
};

} // namespace string19
