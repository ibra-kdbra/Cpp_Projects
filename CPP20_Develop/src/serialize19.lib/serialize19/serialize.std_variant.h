#pragma once
#include "serialize.h"

#include <meta19/nullptr_to.h>
#include <stddef.h> // size_t
#include <variant>

namespace serialize19 {

using meta19::nullptr_to;

template<Archive A, class... Ts> void serialize(A& a, std::variant<Ts...>& variant) {
    auto index = static_cast<uint32_t>(variant.index());
    serialize(a, index);

    auto entry = [&a, &variant, &index]<class Value>(Value*, size_t i) {
        if (index == i) {
            if constexpr (A::mode == ArchiveMode::Read) {
                auto value = Value{};
                serialize(a, value);
                variant = value;
            }
            else {
                auto& value = *std::get_if<Value>(&variant);
                serialize(a, value);
            }
        }
    };
    [&entry]<class... Vs, size_t... Is>(std::variant<Vs...>*, std::index_sequence<Is...>*) {
        (entry(nullptr_to<Vs>, Is), ...);
    }(&variant, nullptr_to<std::make_index_sequence<sizeof...(Ts)>>);
}

} // namespace serialize19
