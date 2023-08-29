#pragma once
#include "serialize.h"

#include <vector>

namespace serialize19 {

template<Archive A, class T, class... Ts> void serialize(A& a, std::vector<T, Ts...>& vector) {
    auto size = static_cast<uint32_t>(vector.size());
    serialize(a, size);
    if constexpr (A::mode == ArchiveMode::Read) {
        vector.resize(size);
    }
    if constexpr (std::is_same_v<T, bool>) {
        // note: vector<bool> allows no byte level access
        if constexpr (A::mode == ArchiveMode::Size) {
            auto byteCount = (size + 7u) >> 3;
            a.withSlice(BufferSlice{nullptr, byteCount});
        }
        else if constexpr (A::mode == ArchiveMode::Read) {
            auto it = vector.begin();
            while (size) {
                auto byte = uint8_t{};
                serialize(a, byte);
                for (auto bit = 1u; size > 0u && bit < 256u; bit = bit << 1, size--) {
                    if ((byte & bit) != 0) *it = true;
                    it++;
                }
            }
        }
        else {
            auto it = vector.begin();
            while (size) {
                auto byte = uint8_t{};
                for (auto bit = 1u; size > 0u && bit < 256u; bit = bit << 1, size--) {
                    if (*it) byte |= bit;
                    it++;
                }
                serialize(a, byte);
            }
        }
    }
    else {
        for (auto& value : vector) {
            serialize(a, value);
        }
    }
}

} // namespace serialize19
