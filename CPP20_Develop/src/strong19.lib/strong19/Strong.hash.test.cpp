#include "Strong.hash.h"

#include <gtest/gtest.h>

using namespace strong19;

DEFINE_STRONG(Name, std::string);
DEFINE_STRONG(Velocity, double);

TEST(Strong_hash, string) {
    auto a = hashFor<Name>{};
    auto b = std::hash<StrongValueType<Name>>{};

    auto s = Name{"Hello"};
    EXPECT_EQ(a(s), b(s.v));
}

TEST(Strong_hash, double) {
    auto a = hashFor<Velocity>{};
    auto b = std::hash<StrongValueType<Velocity>>{};

    auto v = Velocity{1};
    EXPECT_EQ(a(v), b(v.v));
}
