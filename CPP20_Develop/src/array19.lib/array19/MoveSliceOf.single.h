#pragma once
#include "MoveSliceOf.h"

namespace array19 {

/// view a single object as a move slice of one element
template<class T> static constexpr auto moveSliceOfSingle(T&& data) noexcept -> MoveSliceOf<T> {
    return MoveSliceOf{&data, 1};
}

} // namespace array19
