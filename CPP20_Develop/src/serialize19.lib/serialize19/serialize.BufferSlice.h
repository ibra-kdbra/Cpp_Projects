#pragma once
#include "BufferSlice.h"
#include "serialize.h"

namespace serialize19 {

template<Archive A> void serialize(A& a, BufferSlice& slice) {
    auto count = static_cast<uint32_t>(slice.count());
    serialize(a, count);
    if constexpr (A::mode == ArchiveMode::Read) {
        slice = a.withSlice(count);
    }
    else {
        a.withSlice(slice);
    }
}

} // namespace serialize19
