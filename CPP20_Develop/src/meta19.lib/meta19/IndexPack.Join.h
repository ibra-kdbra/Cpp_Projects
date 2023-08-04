#pragma once
#include "RemoveReference.h"
#include "nullptr_to.h"

#include <stddef.h> // size_t

namespace meta19 {

template<template<size_t...> class Tpl, size_t... Ls, size_t... Rs>
constexpr auto indexPackJoinType(Tpl<Ls...>*, Tpl<Rs...>*) -> Tpl<Ls..., Rs...>;

template<class L, class R> using IndexPackJoin = decltype(indexPackJoinType(nullptr_to<L>, nullptr_to<R>));

} // namespace meta19
