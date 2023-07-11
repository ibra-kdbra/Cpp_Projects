#pragma once
#include <type_traits>

namespace mage_hand {
template<class T> using RemoveCvRef = std::remove_cv_t<std::remove_reference_t<T>>;

template<class T> struct Overload {
    template<bool value> using If = std::enable_if_t<value, T>;
};

template<bool value> using If = std::enable_if_t<value, bool>;
}
