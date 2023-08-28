#pragma once
#include <stddef.h> // size_t

namespace partial19 {

/// Minimal constexpr enabled implemenation of std::bitset
/// Only the mandatory methods are implemented
template<size_t Count> struct Bitset {
private:
    using T = unsigned;
    constexpr static auto BitsPerT = 8 * sizeof(T);
    constexpr static auto Len = (Count + BitsPerT - 1) / BitsPerT;

    T m[Len] = {};

public:
    constexpr static auto count = Count;

    constexpr bool operator==(const Bitset& o) const = default;

    [[nodiscard]] constexpr bool operator[](size_t index) const noexcept {
        return 0 != (m[index / BitsPerT] & (1u << (index % BitsPerT)));
    }

    constexpr void reset() noexcept {
        for (auto& v : m) v = {};
    }

    constexpr void setAt(size_t index) noexcept { m[index / BitsPerT] |= (1u << (index % BitsPerT)); }
    constexpr void resetAt(size_t index) noexcept { m[index / BitsPerT] &= ~static_cast<T>(1u << (index % BitsPerT)); }
};

} // namespace partial19
