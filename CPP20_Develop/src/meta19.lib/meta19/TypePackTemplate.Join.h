#pragma once
#include "nullptr_to.h"

namespace meta19 {

namespace details {

template<class T> struct Join;

template<template<class...> class Tpl, class... Ts> struct Join<Tpl<Ts...>> {};

template<template<class...> class Tpl, class... Ts, class... Ps>
auto operator+(const Join<Tpl<Ts...>>&, const Join<Tpl<Ps...>>&) -> Join<Tpl<Ts..., Ps...>> {
    return {};
}

template<template<class...> class Tpl, class... Ts> auto unwrap(const Join<Tpl<Ts...>>&) -> Tpl<Ts...>;

} // namespace details

template<class... Ps> using JoinTypePacks = decltype(details::unwrap((*nullptr_to<details::Join<Ps>> + ...)));

} // namespace meta19
