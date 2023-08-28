#pragma once
#include "array19/SliceOf.h"

#include <stdint.h> // uint8_t

namespace serialize19 {

using array19::SliceOf;
using BufferSlice = SliceOf<const uint8_t>;

} // namespace serialize19
