#pragma once
#include "Type.h"
#include "TypePack.h"

#include <type_traits>

namespace meta19 {

template<template<class...> class Tpl, class... Ts, class T>
constexpr auto typePackContains(Tpl<Ts...>* = {}, Type<T>* = {}) {
    return (std::is_same_v<Ts, T> || ...);
}

template<class Tp, class T> constexpr bool type_pack_contains = typePackContains(nullptr_to<Tp>, type<T>);

} // namespace meta19
