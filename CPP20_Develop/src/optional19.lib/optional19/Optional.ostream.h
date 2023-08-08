#pragma once
#include "Optional.h"

#include <ostream>

namespace optional19 {

template<class T> auto operator<<(std::ostream& out, const Optional<T>& opt) -> std::ostream& {
    if (opt)
        return out << opt.value();
    else
        return out << "nullopt";
}

} // namespace optional19
