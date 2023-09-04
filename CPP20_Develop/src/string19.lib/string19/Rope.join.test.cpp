#include "Rope.Decimal.h"
#include "Rope.StringView.h"
#include "Rope.char.h"
#include "Rope.join.h"
#include "Rope.store.h"
#include "StringStore.literal.h"
#include "StringView.equals.h"
#include "StringView.literal.h"
#include "StringView.store.h"

#include <gtest/gtest.h>

using namespace string19;

TEST(Rope, joinEmpty) {
    constexpr auto rope = Separator{','}.join();
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);
    static_assert(viewStore(store) == viewLiteral(""));
    EXPECT_EQ(store, storeLiteral(""));
}

TEST(Rope, joinChar) {
    constexpr auto rope = Separator{','}.join('1', '2', '3');
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);
    static_assert(viewStore(store) == viewLiteral("1,2,3"));
    EXPECT_EQ(store, storeLiteral("1,2,3"));
}

TEST(Rope, joinView) {
    constexpr auto rope = Separator{viewLiteral(", ")}.join(viewLiteral("1"), '2', '3');
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);
    static_assert(viewStore(store) == viewLiteral("1, 2, 3"));
    EXPECT_EQ(store, storeLiteral("1, 2, 3"));
}

TEST(Rope, joinDecimal) {
    constexpr auto rope = Separator{viewLiteral(", ")}.join(decimal<1>, decimal<-2>, decimal<3>);
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);
    static_assert(viewStore(store) == viewLiteral("1, -2, 3"));
    EXPECT_EQ(store, storeLiteral("1, -2, 3"));
}
