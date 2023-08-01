#pragma once
#include "Enum.h"

namespace enum19 {

template<class T>
constexpr auto max_underlying_value_of = []() -> std::underlying_type_t<T> {
    using V = std::underlying_type_t<T>;
    auto result = V{};
    for (auto& member : meta_enum_for<T>.members) {
        const auto v = static_cast<V>(member.value);
        if (result < v) result = v;
    }
    return result;
}();

} // namespace enum19
