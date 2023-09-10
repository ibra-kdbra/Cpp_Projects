#pragma once
#include "Strong.h"

#include <functional> // std::hash
#include <stddef.h> // size_t

namespace strong19 {

template<class S> struct hashFor {
    size_t operator()(const S& s) const { return std::hash<StrongValueType<S>>{}(s.v); }
};

} // namespace strong19
