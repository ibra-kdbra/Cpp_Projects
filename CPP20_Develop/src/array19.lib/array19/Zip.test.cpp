#include "Zip.h"

#include "Array.h"

using namespace array19;

void constexpr_Zip_test() {
    constexpr auto sum = [] {
        auto a = Array{1, 2, 3};
        unsigned b[] = {4, 5, 6};
        auto r = 0;
#ifdef _MSC_VER
        auto z = Zip{a, b}; // cl crashes if we inline this && run it as constexpr
        for (auto [va, vb] : z) {
#else
        for (auto [va, vb] : Zip{a, b}) {
#endif
            r += va * vb;
        }
        return r;
    }();
    static_assert(sum == (1 * 4) + (2 * 5) + (3 * 6));
}
