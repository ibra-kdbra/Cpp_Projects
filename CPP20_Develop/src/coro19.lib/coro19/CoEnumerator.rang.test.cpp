#include "CoEnumerator.range.h"

#include <gtest/gtest.h>

using namespace coro19;

auto someValues() -> CoEnumerator<int> {
    co_yield 12;
    co_yield 23;
}

TEST(CoEnumeratorRange, some) {
    auto sum = int{};
    for (auto v : someValues()) sum += v;
    EXPECT_EQ(sum, 12 + 23);
}
