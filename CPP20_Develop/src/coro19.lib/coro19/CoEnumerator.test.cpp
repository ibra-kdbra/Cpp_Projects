#include "CoEnumerator.h"

#include <gtest/gtest.h>

using namespace coro19;

auto statusCounter(int& status) -> CoEnumerator<int> {
    struct finally {
        int& status;
        ~finally() { status += 1; }
    };
    // change the status when the frame is going to be destroyed
    auto on_return = finally{status};

    status += 1;
    co_yield 12;
}

TEST(CoEnumerator, statusCounter) {
    auto status = int{};
    auto e = statusCounter(status);
    EXPECT_EQ(status, 0);
    ASSERT_TRUE(e);
    ++e;
    ASSERT_TRUE(e);
    EXPECT_EQ(*e, 12);
    EXPECT_EQ(status, 1);
    ++e;
    ASSERT_FALSE(e);
    EXPECT_EQ(status, 2); // frame is already destroyed
}

auto noYield(bool c = false) -> CoEnumerator<int> {
    if (c) co_yield 23;
}

TEST(CoEnumerator, noYield) {
    auto e = noYield();
    ASSERT_TRUE(e);
    ++e;
    ASSERT_FALSE(e);
}
