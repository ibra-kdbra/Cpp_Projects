#pragma once
#include "partial19/Partial.h" // requires partial19
#include "serialize.h"

#include <cstdint>
#include <stddef.h> // size_t

namespace serialize19 {

template<Archive A, size_t size> void serialize(A& a, partial19::Bitset<size>& bitset) {
    if (a.mode == ArchiveMode::Read) {
        for (auto i = 0u; i < size;) {
            auto chunk = uint8_t{};
            serialize(a, chunk);
            for (auto mask = 0x80u; mask != 0 && i < size; mask >>= 1, i++) {
                if (0 != (chunk & mask)) {
                    bitset.setAt(i);
                }
                else {
                    bitset.resetAt(i);
                }
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

template<Archive A, class... Ts> void serialize(A& a, partial19::Partial<Ts...>& partial) {
    using meta19::Type;
    using T = partial19::Partial<Ts...>;
    auto whichBits = static_cast<typename T::Bits>(partial.which());
    serialize(a, whichBits);

    if constexpr (A::mode == ArchiveMode::Read) {
        partial = T::fromFactory(
            [&](size_t i) { return whichBits[i]; },
            [&]<class T>(Type<T>*, void* ptr) {
                auto* value = new (ptr) T{};
                serialize(a, *value);
            });
    }
    else {
        partial.visitInitialized([&](auto& value) { serialize(a, value); });
    }
}

} // namespace serialize19
