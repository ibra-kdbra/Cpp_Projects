#pragma once
#include <stddef.h> // size_t

namespace meta19 {

template<size_t V, size_t... Vs> inline constexpr auto pack_back = (V, ..., Vs);

template<class> inline constexpr auto index_pack_back = nullptr;

template<template<size_t...> class Tpl, size_t V, size_t... Vs>
inline constexpr auto index_pack_back<Tpl<V, Vs...>> = (V, ..., Vs);

} // namespace meta19
