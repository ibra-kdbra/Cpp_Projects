#pragma once

namespace meta19 {

template<class T> constexpr auto type_pack_count = nullptr;

template<template<class...> class Tpl, class... Ts> constexpr auto type_pack_count<Tpl<Ts...>> = sizeof...(Ts);

} // namespace meta19
