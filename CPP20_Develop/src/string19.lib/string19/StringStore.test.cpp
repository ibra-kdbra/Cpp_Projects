#include "StringStore.h"

#include <gtest/gtest.h>

using namespace string19;

TEST(StringStore, basic) {
    constexpr auto x = StringStore{"Hello"};
    static_assert(x[0] == 'H');
    static_assert(x.count == 6u); // 5 chars + '\0'

    EXPECT_EQ(x[0], 'H');
    EXPECT_EQ(x.count, 6u);
}
