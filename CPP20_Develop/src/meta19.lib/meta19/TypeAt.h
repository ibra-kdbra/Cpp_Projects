#pragma once
#include "Index.h"
#include "nullptr_to.h"

#include <stddef.h> // size_t
#include <utility> // std::index_sequence

namespace meta19 {

namespace details {

template<size_t, class> struct IndexType {};

template<class IP, class... Ts> struct IndexTypeOverloads;
template<size_t... Is, class... Ts>
struct IndexTypeOverloads<std::index_sequence<Is...>, Ts...> : IndexType<Is, Ts>... {};

template<size_t I, class T> auto typeAtMap(IndexType<I, T>*) -> T;
template<class T, size_t I> auto indexTypeOfMap(IndexType<I, T>*) -> Index<I>;
template<class T, size_t I> constexpr auto indexOfMap(IndexType<I, T>*) -> size_t { return I; }

} // namespace details

/// Generate a index+type lookup map for the given type pack Ts
template<class... Ts> using IndexTypeMap = details::IndexTypeOverloads<std::make_index_sequence<sizeof...(Ts)>, Ts...>;

/// type at given index I in the given type pack Ts
template<size_t I, class... Ts> using TypeAt = decltype(details::typeAtMap<I>(nullptr_to<IndexTypeMap<Ts...>>));

/// type at given index I using the given lookup map ITM
template<size_t I, class ITM> using TypeAtMap = decltype(details::typeAtMap<I>(nullptr_to<ITM>));

/// type wrapped index of given type T in the given type pack Ts
template<class T, class... Ts> using IndexOf = decltype(details::indexTypeOfMap<T>(nullptr_to<IndexTypeMap<Ts...>>));

/// type wrapped index of given type T in given lookup map ITM
template<class T, class ITM> using IndexOfMap = decltype(details::indexTypeOfMap<T>(nullptr_to<ITM>));

/// index value of given type T in the given type pack Ts
template<class T, class... Ts> inline constexpr auto index_of = details::indexOfMap<T>(nullptr_to<IndexTypeMap<Ts...>>);

/// index value of given type T in the given lookup map ITM
template<class T, class ITM> inline constexpr auto index_of_map = details::indexOfMap<T>(nullptr_to<ITM>);

} // namespace meta19
