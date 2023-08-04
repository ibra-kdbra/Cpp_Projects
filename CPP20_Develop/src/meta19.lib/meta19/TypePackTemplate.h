#pragma once
#include "nullptr_to.h"

#include <stddef.h> // size_t

namespace meta19 {

template<template<class...> class> struct TypePackTemplate;

template<template<class...> class Tpl> inline constexpr auto& type_pack_template = nullptr_to<TypePackTemplate<Tpl>>;

} // namespace meta19
