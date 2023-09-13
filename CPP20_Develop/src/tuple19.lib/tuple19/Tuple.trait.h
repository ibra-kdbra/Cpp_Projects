#pragma once
#include "Tuple.h"

namespace tuple19 {

template<class T> constexpr auto is_tuple = false;
template<class... Ts> constexpr auto is_tuple<Tuple<Ts...>> = true;

} // namespace tuple19
