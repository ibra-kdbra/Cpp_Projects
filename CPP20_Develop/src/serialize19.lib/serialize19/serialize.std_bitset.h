#pragma once
#include "serialize.h"

#include <bitset>
#include <stddef.h> // size_t

namespace serialize19 {

template<Archive A, size_t size> void serialize(A& a, std::bitset<size>& bitset) {
    if (a.mode == ArchiveMode::Read) {
        for (auto i = 0u; i < size;) {
            auto chunk = uint8_t{};
            serialize(a, chunk);
            for (auto mask = 0x80u; mask != 0 && i < size; mask >>= 1, i++) {
                bitset[i] = chunk & mask;
            }
        }
    }
    else {
        for (auto i = 0u; i < size;) {
            auto chunk = uint8_t{};
            for (auto mask = 0x80u; mask != 0 && i < size; mask >>= 1, i++) {
                chunk |= mask * bitset[i];
            }
            serialize(a, chunk);
        }
    }
}

} // namespace serialize19
