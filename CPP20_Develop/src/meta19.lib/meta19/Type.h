#pragma once
#include "nullptr_to.h"

namespace meta19 {

/// wrapper for any C++ type
/// notes:
/// - nesting Type<Type<X>> is not allowed
/// - intentionally left without implementation to avoid compiler overhead for that
template<class> struct Type;
template<class T> struct Type<Type<T>> {
    static_assert(sizeof(T) && false, "nested Type!");
};

/// type value wrapper for any C++ type
/// notes:
/// - unlike blank nullptr_to this allows references as well
template<class T> static constexpr auto& type [[maybe_unused]] = nullptr_to<Type<T>>;

} // namespace meta19
