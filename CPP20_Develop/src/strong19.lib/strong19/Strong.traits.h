#pragma once
#include "Strong.h"
#include "meta19/TypePackTemplate.contains.h"

#include <type_traits>

namespace strong19 {

using meta19::type_pack_contains;

template<class S, class V> concept has_strong_value = std::is_same_v<StrongValueType<S>, V>;

template<class S, class... Tags> concept has_strong_tags = (type_pack_contains<StrongTags<S>, Tags> && ...);

} // namespace strong19
