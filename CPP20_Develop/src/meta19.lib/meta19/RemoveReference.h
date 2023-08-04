#pragma once
#include "Type.h"

namespace meta19 {

template<class T> constexpr auto pointeeTypeOf(Type<T*>*) -> T;
template<class T> using PointeeOf = decltype(pointeeTypeOf(type<T>));

template<class T> constexpr auto referencedTypeOf(Type<T&>*) -> T;
template<class T> constexpr auto referencedTypeOf(Type<T&&>*) -> T;
template<class T> using ReferencedOf = decltype(referencedTypeOf(type<T>));

template<class T> constexpr auto storedTypeOf(Type<T>*) -> T;
template<class T> constexpr auto storedTypeOf(Type<T&>*) -> T;
template<class T> constexpr auto storedTypeOf(Type<T&&>*) -> T;
template<class T> constexpr auto storedTypeOf(Type<const T>*) -> T;
template<class T> constexpr auto storedTypeOf(Type<const T&>*) -> T;
template<class T> constexpr auto storedTypeOf(Type<const T&&>*) -> T;
template<class T> using StoredOf = decltype(storedTypeOf(type<T>));

} // namespace meta19
