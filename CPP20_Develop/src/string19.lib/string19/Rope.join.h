#pragma once
#include "Rope.h"

namespace string19 {

template<class V> struct Separator {
    V v;

    template<class T, class... Ts> constexpr auto join(T t, Ts... args) { return Rope{t, Rope{v, args}...}; }
    constexpr auto join() { return Rope{}; }
};

template<class V> Separator(V) -> Separator<V>;

} // namespace string19
