#include "None.h"

#include "None.ostream.h"

#include <gtest/gtest.h>

using namespace variant19;

TEST(None, example) {
    auto n = None{};

    ASSERT_EQ(n, None{});
}
