#pragma once
#include "array19/Array.h"

#include <stdint.h> // uint8_t

namespace serialize19 {

using array19::Array;

/// use by SliceReader & WriteAppender to mark change in endianess
/// note: compare host to target endianness
enum class EndianBehaviour : bool { Keep, Flip };

/// perform endian flip for buffer at data and type T
template<class T> auto endianFlipFor(T input) -> T {
    constexpr auto N = sizeof(T);
    if constexpr (N == 1)
        return input;
    else if constexpr (N == 2) {
        auto value = *(uint16_t*)&input;
        value = (uint16_t)(value << 8) | (value >> 8);
        return *(T*)&value;
    }
    else if constexpr (N == 4) {
        auto value = *(uint32_t*)&input;
        value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
        value = (value << 16) | (value >> 16);
        return *(T*)&value;
    }
    else if constexpr (N == 8) {
        auto value = *(uint64_t*)&input;
        value = ((value << 8) & 0xFF00FF00FF00FF00) | ((value >> 8) & 0x00FF00FF00FF00FF);
        value = ((value << 16) & 0xFFFF0000FFFF0000) | ((value >> 16) & 0x0000FFFF0000FFFF);
        value = (value << 32) | (value >> 32);
        return *(T*)&value;
    }
    else {
        auto array = Array<uint8_t, N>{};
        for (auto i = 0u; i < N; i++) array.amend()[i] = ((uint8_t*)&input)[N - i - 1];
        return *(T*)array.m;
    }
}

} // namespace serialize19
