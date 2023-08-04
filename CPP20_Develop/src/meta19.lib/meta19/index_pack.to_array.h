#pragma once
#include "array19/Array.h"

#include <stddef.h> // size_t

namespace meta19 {

template<size_t... Is> inline constexpr auto pack_to_array = array19::Array{Is...};

template<class> inline constexpr auto index_pack_to_array = nullptr;

template<template<size_t...> class Tpl, size_t... Is>
inline constexpr auto index_pack_to_array<Tpl<Is...>> = array19::Array{Is...};

} // namespace meta19
