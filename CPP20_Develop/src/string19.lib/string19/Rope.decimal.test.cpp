#include "Rope.Decimal.h"
#include "Rope.Rope.h"
#include "Rope.StringView.h"
#include "Rope.store.h"
#include "StringStore.literal.h"
#include "StringView.equals.h"
#include "StringView.literal.h"
#include "StringView.store.h"

#include <gtest/gtest.h>

using namespace string19;

TEST(Rope_decimal, example) {
    constexpr auto rope = Rope{viewLiteral("dec: "), decimal<13>};
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);

    static_assert(viewStore(store) == viewLiteral("dec: 13"));
    EXPECT_EQ(store, storeLiteral("dec: 13"));
}

TEST(Rope_decimal, negative) {
    constexpr auto rope = Rope{viewLiteral("dec: "), decimal<-13>};
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);

    static_assert(viewStore(store) == viewLiteral("dec: -13"));
    EXPECT_EQ(store, storeLiteral("dec: -13"));
}
