#pragma once
#include "IndexPackTemplate.h"
#include "nullptr_to.h"

#include <stddef.h> // size_t

namespace meta19 {

template<template<size_t...> class ToTpl, template<size_t...> class FromTpl, size_t... Vs>
constexpr auto changeIndexPackTemplateType(FromTpl<Vs...>*, IndexPackTemplate<ToTpl>* = {}) -> ToTpl<Vs...>;

template<template<size_t...> class ToTpl, class From> //
using ChangeIndexPackTemplate = decltype(changeIndexPackTemplateType<ToTpl>(nullptr_to<From>));

} // namespace meta19
