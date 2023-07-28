#pragma once
#include "SliceOf.h"

namespace array19 {

/// view a single object as a slice of one element
template<class T> static constexpr auto sliceOfSingle(T& data) noexcept -> SliceOf<T> { return SliceOf{&data, 1}; }

} // namespace array19
