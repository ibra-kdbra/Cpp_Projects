#pragma once
#include "serialize.h"

#include <map>

namespace serialize19 {

template<Archive A, class K, class... Ts> void serialize(A& a, std::map<K, Ts...>& map) {
    auto size = static_cast<uint32_t>(map.size());
    serialize(a, size);
    if constexpr (A::mode == ArchiveMode::Read) {
        for (auto i = 0u; i < size; i++) {
            auto key = K{};
            serialize(a, key);
            serialize(a, map[key]);
        }
    }
    else {
        for (auto& [key, value] : map) {
            serialize(a, key);
            serialize(a, value);
        }
    }
}

} // namespace serialize19
