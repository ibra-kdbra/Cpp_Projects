#pragma once
#include "Type.h"
#include "nullptr_to.h"

namespace meta19 {

template<class T> constexpr auto typeUnwrapType(Type<T>*) -> T;
template<class T> using TypeUnwrap = decltype(typeUnwrapType(nullptr_to<T>));

} // namespace meta19
