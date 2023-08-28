#pragma once
#include "optional19/Optional.h"
#include "serialize.h"

#include <utility>

namespace serialize19 {

template<Archive A, class T> void serialize(A& a, optional19::Optional<T>& optional) {
    auto hasValue = static_cast<bool>(optional);
    serialize(a, hasValue);
    if constexpr (A::mode == ArchiveMode::Read) {
        if (hasValue) {
            auto value = T{};
            serialize(a, value);
            optional = std::move(value);
        }
        else {
            optional = {};
        }
    }
    else {
        if (hasValue) {
            serialize(a, optional.value());
        }
    }
}

} // namespace serialize19
