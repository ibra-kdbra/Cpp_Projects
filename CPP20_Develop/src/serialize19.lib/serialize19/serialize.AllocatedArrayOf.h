#pragma once
#include "array19/AllocatedArrayOf.h"
#include "serialize.h"

#include <stdint.h> // uint32_t

namespace serialize19 {

using array19::AllocatedArrayOf;

template<Archive A, class T> void serialize(A& a, AllocatedArrayOf<T>& allocatedArray) {
    auto count = static_cast<uint32_t>(allocatedArray.count());
    serialize(a, count);
    if constexpr (A::mode == ArchiveMode::Read) {
        allocatedArray = AllocatedArrayOf<T>::createCount(count);
        for (auto i = 0U; i < count; i++) {
            serialize(a, allocatedArray.amend()[i]);
        }
    }
    else {
        for (auto& value : allocatedArray) serialize(a, value);
    }
}

} // namespace serialize19
