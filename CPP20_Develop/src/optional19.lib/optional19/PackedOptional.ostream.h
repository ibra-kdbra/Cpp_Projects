#pragma once
#include "PackedOptional.h"

#include <ostream>

namespace optional19 {

template<auto m, auto i> auto operator<<(std::ostream& out, const PackedOptional<m, i>& opt) -> std::ostream& {
    if (opt)
        return out << opt.value();
    else
        return out << "nullopt";
}

} // namespace optional19
