#include "WithIndex.h"

using namespace array19;

void constexpr_WithIndex_test() {
    constexpr auto sum = [] {
        int a[] = {1, 2, 3};
        auto r = 0;
#ifdef _MSC_VER
        auto wi = WithIndex{a}; // cl crashes if we inline this && run it as constexpr
        for (auto [v, i] : wi) {
#else
        for (auto [v, i] : WithIndex{a}) {
#endif
            r += v * i;
        }
        return r;
    }();
    static_assert(sum == (1 * 0) + (2 * 1) + (3 * 2));
}
