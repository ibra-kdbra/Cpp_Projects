#pragma once
#include "Optional.h"

namespace optional19 {

template<class T> constexpr auto operator==(const Optional<T>& a, const Optional<T>& b) {
    return (bool)a ? (bool)b && a.value() == b.value() : !(bool)b;
}
template<class T> constexpr auto operator!=(const Optional<T>& a, const Optional<T>& b) { return !(a == b); }

} // namespace optional19
