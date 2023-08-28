#include "Enum.max.h"

#include <gtest/gtest.h>

using namespace enum19;

namespace my_test {

ENUM19(Color, int, green = 0x0f0, red = 0xf00, brown = green + red);
ENUM19(List, int, Base, Apple, Banana, Orange, Pinapple);

} // namespace my_test

TEST(Enum_max, example) {
    static_assert(enum19::max_underlying_value_of<my_test::Color> == 0xff0);
    static_assert(enum19::max_underlying_value_of<my_test::List> == 4);
}
