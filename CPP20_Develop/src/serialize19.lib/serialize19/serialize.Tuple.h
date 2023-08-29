#pragma once
#include "serialize.h"
#include "tuple19/Tuple.h" // requires tuple19

namespace serialize19 {

template<Archive A, class... Ts> void serialize(A& a, tuple19::Tuple<Ts...>& tuple) {
    using meta19::type;
    (serialize(a, tuple.amendOf(type<Ts>)), ...);
}

} // namespace serialize19
