#pragma once
#include "TypePackTemplate.h"
#include "nullptr_to.h"

namespace meta19 {

template<template<class...> class ToTpl, template<class...> class FromTpl, class... Ts>
constexpr auto changeTypePackTemplateType(FromTpl<Ts...>*, TypePackTemplate<ToTpl>* = {}) -> ToTpl<Ts...>;

template<template<class...> class ToTpl, class From> //
using ChangeTypePackTemplate = decltype(changeTypePackTemplateType<ToTpl>(nullptr_to<From>));

} // namespace meta19
