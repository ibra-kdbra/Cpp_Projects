#pragma once
#include "serialize.h"

#include <tuple>

namespace serialize19 {

template<Archive A, class... Ts> void serialize(A& a, std::tuple<Ts...>& tuple) {
    (serialize(a, std::get<Ts>(tuple)), ...);
}

} // namespace serialize19
