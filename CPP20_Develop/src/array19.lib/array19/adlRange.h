#pragma once
#include <array> // std::begin, std::end

namespace array19 {

template<class T> constexpr auto adlBegin(T& v) {
    using std::begin;
    return begin(v);
}
template<class T> constexpr auto adlEnd(T& v) {
    using std::end;
    return end(v);
}

} // namespace array19