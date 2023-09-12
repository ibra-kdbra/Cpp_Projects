#pragma once
#include "Tuple.h"
#include "meta19/Index.h"

#include <stddef.h> // size_t
#include <utility>

namespace tuple19 {

using meta19::_index;

template<size_t I, class... Ts> constexpr auto get(Tuple<Ts...>& tuple) -> decltype(auto) {
    return tuple.amendAt(_index<I>);
}
template<size_t I, class... Ts> constexpr auto get(Tuple<Ts...>&& tuple) -> decltype(auto) {
    return std::move(tuple.amendAt(_index<I>));
}
template<size_t I, class... Ts> constexpr auto get(const Tuple<Ts...>& tuple) -> decltype(auto) {
    return tuple.at(_index<I>);
}
template<class T, class... Ts> constexpr auto get(Tuple<Ts...>& tuple, Type<T>* = {}) -> decltype(auto) {
    return tuple.amendOf(type<T>);
}
template<class T, class... Ts> constexpr auto get(Tuple<Ts...>&& tuple, Type<T>* = {}) -> decltype(auto) {
    return std::move(tuple.amendOf(type<T>));
}
template<class T, class... Ts> constexpr auto get(const Tuple<Ts...>& tuple, Type<T>* = {}) -> decltype(auto) {
    return tuple.of(type<T>);
}

} // namespace tuple19

/// for c++17 structured bindings support
template<class... Ts> class std::tuple_size<tuple19::Tuple<Ts...>> {
public:
    static constexpr auto value = sizeof...(Ts);
};

template<size_t I, class... Ts> class std::tuple_element<I, tuple19::Tuple<Ts...>> {
public:
    using type = typename tuple19::Tuple<Ts...>::template At<I>;
};
