#include "StringView.h"

#include "StringView.literal.h"

#include <gtest/gtest.h>
#include <string_view>

using namespace string19;

TEST(StringView, fromLiteral) {
    constexpr auto x = viewLiteral("Hello");
    static_assert(x[0] == 'H');
    static_assert(x.count == 5u);

    EXPECT_EQ(x[0], 'H');
    EXPECT_EQ(x.count, 5u);
    EXPECT_EQ(x.extraZero, ExtraZero::Present);

    auto y = viewLiteral("World"); // not constexpr
    EXPECT_EQ(y[0], 'W');
    EXPECT_EQ(y.count, 5u);
    EXPECT_EQ(x.extraZero, ExtraZero::Present);

    static auto z = x; // runtime copy
    EXPECT_EQ(z[0], 'H');
    EXPECT_EQ(z.count, 5u);
    EXPECT_EQ(x.extraZero, ExtraZero::Present);
}
