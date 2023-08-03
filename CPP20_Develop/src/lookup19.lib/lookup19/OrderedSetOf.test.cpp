#include "OrderedSetOf.h"

#include <gtest/gtest.h>

using namespace lookup19;

TEST(OrderedSetOf, intExample) {

    auto v = OrderedSetOf<int>{};

    EXPECT_TRUE(v.isEmpty());
    EXPECT_EQ(v.totalCapacity(), 0u);
    EXPECT_EQ(v.unusedCapacity(), 0u);
    ASSERT_EQ(v.count(), 0u);

    v.ensureUnusedCapacity(1);

    EXPECT_GE(v.totalCapacity(), 1u);
    EXPECT_GE(v.unusedCapacity(), 1u);
    ASSERT_EQ(v.count(), 0u);

    v.insert(12);
    v.insert(23);

    ASSERT_EQ(v.count(), 2u);
    EXPECT_GE(v.totalCapacity(), 2u);

    v.insert(42);
    v.insert(12);

    ASSERT_EQ(v.count(), 3u);
    EXPECT_GE(v.totalCapacity(), 3u);

    v.remove(OrderedSliceOf{v}.lowerBound(12));

    ASSERT_EQ(v.count(), 2u);
}
