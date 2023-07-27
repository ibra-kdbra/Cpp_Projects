#include "Array.h"

using namespace array19;

void constexpr_Array_test() {
    constexpr auto a = Array{1, 2, 3};

    static_assert(a.count == 3);
    static_assert(a[1] == 2);

    constexpr auto s = [&] {
        auto s = 0;
        for (auto v : a) s += v;
        return s;
    }();
    static_assert(s == 6);
}

void empty_Array_test() {
    constexpr auto a = Array<int, 0>{};

    static_assert(a.count == 0);
    static_assert(a.isEmpty());

    constexpr auto s = [&] {
        auto s = 0;
        for (auto v : a) s += v;
        return s;
    }();
    static_assert(s == 0);
}

void mutable_Array_test() {
    constexpr auto ca = [] {
        auto a = Array{1, 2, 3};
        a.amend()[1] = 5;
        return a;
    }();

    static_assert(ca == Array{1, 5, 3});
}
