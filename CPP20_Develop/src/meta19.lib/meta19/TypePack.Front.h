#pragma once
#include "TypePack.h"
#include "nullptr_to.h"

namespace meta19 {

template<template<class...> class Tpl, class T, class... Ts> auto typePackFrontType(Tpl<T, Ts...>*) -> T;

template<class... Ts> using PackFront = decltype(typePackFrontType(nullptr_to<TypePack<Ts...>>));

template<class T> using TypePackFront = decltype(typePackFrontType(nullptr_to<T>));

} // namespace meta19
