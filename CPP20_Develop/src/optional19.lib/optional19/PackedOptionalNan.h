#pragma once
#include "PackedOptional.h"

#include <limits> // std::numeric_limits

namespace optional19 {

/// helper function to use quiet NaN (not a number) of floating points as invalid
template<class T> constexpr auto quietNanOf() -> T { return std::numeric_limits<T>::quiet_NaN(); }

/// Only for NaN values == is not true to itself
/// note: std::limits only has a non-constexpr version
template<class T> constexpr bool isValid(const T& v) { return v == v; }

/// Alias for conviently using NanOptionals
template<class T> using PackedOptionalNan = PackedOptional<quietNanOf<T>, isValid<T>>;

} // namespace optional19
