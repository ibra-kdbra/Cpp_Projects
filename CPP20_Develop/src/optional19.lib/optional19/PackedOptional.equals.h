#pragma once
#include "PackedOptional.h"

namespace optional19 {

template<auto m, auto i> constexpr auto operator==(const PackedOptional<m, i>& a, const PackedOptional<m, i>& b) {
    return (bool)a ? (bool)b && a.value() == b.value() : !(bool)b;
}
template<auto m, auto i> constexpr auto operator!=(const PackedOptional<m, i>& a, const PackedOptional<m, i>& b) {
    return !(a == b);
}

} // namespace optional19
