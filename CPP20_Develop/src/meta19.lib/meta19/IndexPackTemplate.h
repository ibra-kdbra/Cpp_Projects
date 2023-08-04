#pragma once
#include "nullptr_to.h"

#include <stddef.h> // size_t

namespace meta19 {

template<template<size_t...> class> struct IndexPackTemplate;

template<template<size_t...> class Tpl> inline constexpr auto& index_pack_template = nullptr_to<IndexPackTemplate<Tpl>>;

} // namespace meta19
