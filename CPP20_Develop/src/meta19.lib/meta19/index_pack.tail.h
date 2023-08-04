#pragma once
#include "index_pack.h"

#include <stddef.h> // size_t

namespace meta19 {

template<size_t H, size_t... Vs> inline constexpr auto& pack_tail = index_pack<Vs...>;

template<class> inline constexpr auto index_pack_tail = nullptr;

template<template<size_t...> class Tpl, size_t H, size_t... Vs>
inline constexpr auto& index_pack_tail<Tpl<H, Vs...>> = index_pack<Vs...>;

} // namespace meta19
