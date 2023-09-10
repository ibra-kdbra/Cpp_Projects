#pragma once
#include "Strong.h"

namespace strong19 {

template<class T> constexpr auto weak_of = nullptr_to<T>;

template<class T> requires(is_strong<T>) constexpr auto weak_of<T> = nullptr_to<StrongValueType<T>>;

template<class T> using WeakOf = std::remove_reference_t<decltype(*weak_of<T>)>;

} // namespace strong19
