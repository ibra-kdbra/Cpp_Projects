#pragma once
#include "ADL.h"

#include <stddef.h> // size_t

namespace string19 {

constexpr auto ropeCount(ADL*, char) -> size_t { return 1; }

constexpr void ropeAppend(ADL*, char*& data, char c) { *data++ = c; }

} // namespace string19
