#include "Strong.extras.ostream.h"

#include <gtest/gtest.h>

using namespace strong19;

DEFINE_STRONG(Distance, int);

TEST(Strong, extras_ostream) {
    auto ss = std::stringstream{};
    ss << Distance{1};
    EXPECT_EQ(ss.str(), "Distance{1}");
}
