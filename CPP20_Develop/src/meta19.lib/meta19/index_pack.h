#pragma once
#include "IndexPack.h"
#include "nullptr_to.h"

#include <stddef.h> // size_t

namespace meta19 {

template<size_t... Vs> inline constexpr auto& index_pack = nullptr_to<IndexPack<Vs...>>;

} // namespace meta19
