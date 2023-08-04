#pragma once
#include "TypePack.h"
#include "nullptr_to.h"

namespace meta19 {

template<class... Ts> inline constexpr auto& type_pack = nullptr_to<TypePack<Ts...>>;

} // namespace meta19
