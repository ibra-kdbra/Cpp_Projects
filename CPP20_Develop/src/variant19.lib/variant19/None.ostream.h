#pragma once
#include "None.h"

#include <ostream>

namespace variant19 {

template<class Chr, class Traits> auto operator<<(std::basic_ostream<Chr, Traits>& out, const None&) -> decltype(out)& {
    return out << "∅";
}

} // namespace variant19
