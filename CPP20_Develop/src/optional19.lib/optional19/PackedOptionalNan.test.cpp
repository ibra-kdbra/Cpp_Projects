#include "PackedOptionalNan.h"

#include "PackedOptional.equals.h"
#include "PackedOptional.ostream.h"

#include <gtest/gtest.h>

using namespace optional19;

TEST(PackedOptional, double) {
    using OptDbl = PackedOptionalNan<double>;

    static_assert(sizeof(OptDbl) == sizeof(double));
    {
        constexpr auto optDbl = OptDbl{};
        ASSERT_FALSE(optDbl);
        ASSERT_EQ(optDbl, OptDbl{});

        EXPECT_DOUBLE_EQ((optDbl || 3.14), 3.14);

        constexpr const auto constOpt = optDbl;
        ASSERT_FALSE(constOpt);
        EXPECT_DOUBLE_EQ(constOpt || 4.14, 4.14);

        ASSERT_FALSE(constOpt && [](double v) { return v > 4.14; });
    }
    {
        constexpr auto optDbl = OptDbl{2.3};
        constexpr const auto constOpt = optDbl;
        ASSERT_TRUE(optDbl);
        ASSERT_EQ(constOpt, OptDbl{2.3});

        EXPECT_DOUBLE_EQ(optDbl.value(), 2.3);
        EXPECT_DOUBLE_EQ(optDbl || 4.2, 2.3);

        auto l = constOpt.map([](double v) { return v * 2.0; });
        EXPECT_DOUBLE_EQ(l, 2.3 * 2.0);

        ASSERT_FALSE(constOpt && [](double v) { return v > 4.2; });
        ASSERT_TRUE(constOpt && [](double v) { return v > 0.0; });
    }

    auto optDbl = OptDbl{};
    ASSERT_NE(optDbl, OptDbl{2.3});
    optDbl = 2.3;
    ASSERT_EQ(optDbl, OptDbl{2.3});
}
