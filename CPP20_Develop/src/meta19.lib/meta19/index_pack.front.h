#pragma once
#include <stddef.h> // size_t

namespace meta19 {

template<size_t H, size_t... Vs> inline constexpr auto pack_front = H;

template<class IP> inline constexpr auto index_pack_front = nullptr;

template<template<size_t...> class Tpl, size_t H, size_t... Vs>
inline constexpr auto index_pack_front<Tpl<H, Vs...>> = H;

} // namespace meta19
