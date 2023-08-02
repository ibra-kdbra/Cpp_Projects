#include "Enum.h"

#include "string19/StringView.equals.h"
#include "string19/StringView.literal.h"
#include "string19/StringView.ostream.h"

#include <gtest/gtest.h>

using namespace enum19;
using string19::viewLiteral;

namespace my_test {

ENUM19(
    Color,
    int,
    red = 0xf00,
    green = 0x0f0,
    blue = 0xfff - red - green,
    last //
);

} // namespace my_test

TEST(Enum, example) {
    constexpr auto metaEnum = meta_enum_for<my_test::Color>;

    static_assert(metaEnum.name == viewLiteral("Color"));
    EXPECT_EQ(metaEnum.name, viewLiteral("Color"));

    static_assert(metaEnum.members.count == 4);

    static_assert(metaEnum.members[0].name == viewLiteral("red"));
    static_assert(metaEnum.members[0].value == static_cast<my_test::Color>(0xf00));
    EXPECT_EQ(metaEnum.members[0].name, viewLiteral("red"));

    static_assert(metaEnum.members[1].name == viewLiteral("green"));
    static_assert(metaEnum.members[1].value == static_cast<my_test::Color>(0x0f0));
    EXPECT_EQ(metaEnum.members[1].name, viewLiteral("green"));

    static_assert(metaEnum.members[2].name == viewLiteral("blue"));
    static_assert(metaEnum.members[2].value == static_cast<my_test::Color>(0x00f));
    EXPECT_EQ(metaEnum.members[2].name, viewLiteral("blue"));

    static_assert(metaEnum.members[3].name == viewLiteral("last"));
    static_assert(metaEnum.members[3].value == static_cast<my_test::Color>(0x010));
    EXPECT_EQ(metaEnum.members[3].name, viewLiteral("last"));
}
