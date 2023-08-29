#pragma once
#include "serialize.h"

#include <string>

namespace serialize19 {

template<Archive A, class T, class... Ts> void serialize(A& a, std::basic_string<T, Ts...>& string) {
    auto size = static_cast<uint32_t>(string.size());
    serialize(a, size);
    if constexpr (A::mode == ArchiveMode::Read) {
        string.resize(size);
    }
    for (auto& value : string) {
        serialize(a, value);
    }
}

} // namespace serialize19
