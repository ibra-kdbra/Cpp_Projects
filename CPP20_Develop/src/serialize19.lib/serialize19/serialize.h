#pragma once
#include "Archive.h"

#include <type_traits> // std::is_arithmetic_v, std::is_enum_v

namespace serialize19 {

/// If you hit this fallback you missed a proper overload
struct SerializeFallback {
    template<class T> SerializeFallback(const T&) {
        static_assert(sizeof(T) != sizeof(T), "Missing serialize overload!");
    }
};
template<class A> void serialize(A&, SerializeFallback) {}

template<class T> concept SerializePrimitive = (!std::is_const_v<T> && (std::is_arithmetic_v<T> || std::is_enum_v<T>));

template<Archive A, SerializePrimitive V> auto serialize(A& a, V& v) { a.withPrimitive(v); }

} // namespace serialize19
