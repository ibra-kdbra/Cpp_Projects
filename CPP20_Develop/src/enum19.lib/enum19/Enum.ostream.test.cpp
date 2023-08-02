#include "Enum.ostream.h"

#include <gtest/gtest.h>
#include <sstream>

using namespace enum19;

ENUM19(BoolEnum, bool, Yes, No);

TEST(EnumOstream, bool) {
    auto os = std::stringstream{};
    auto yes = BoolEnum::Yes;
    auto no = BoolEnum::No;
    os << EnumName{yes} << " or " << EnumName{no};
    EXPECT_EQ(os.str(), "Yes (0) or No (1)");
}

ENUM19(Uint8Enum, uint8_t, None, One, Two, Many);

TEST(EnumOstream, uint8) {
    auto os = std::stringstream{};
    auto one = Uint8Enum::One;
    auto many = Uint8Enum::Many;
    os << EnumName{one} << " then " << EnumName{many};
    EXPECT_EQ(os.str(), "One (1) then Many (3)");
}

ENUM19(Int8Enum, int8_t, None, One, Two, Many);

TEST(EnumOstream, int8) {
    auto os = std::stringstream{};
    auto one = Int8Enum::One;
    auto many = Int8Enum::Many;
    os << EnumName{one} << " then " << EnumName{many};
    EXPECT_EQ(os.str(), "One (1) then Many (3)");
}
