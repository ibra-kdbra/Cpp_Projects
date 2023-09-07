#include "Rope.StringStore.h"
#include "Rope.StringView.h"
#include "Rope.char.h"
#include "Rope.store.h"
#include "StringStore.literal.h"
#include "StringView.equals.h"
#include "StringView.literal.h"
#include "StringView.store.h"

#include <gtest/gtest.h>

using namespace string19;

TEST(Rope, example) {
    constexpr auto world = viewLiteral("World");
    constexpr auto stored = storeLiteral("stored");

    constexpr auto rope = Rope{viewLiteral("Hello"), ' ', stored, ' ', world};
    constexpr auto N = ropeCount(rope);
    constexpr auto store = ropeStore<N>(rope);
    static_assert(viewStore(store) == viewLiteral("Hello stored World"));
    EXPECT_EQ(store, storeLiteral("Hello stored World"));
}
