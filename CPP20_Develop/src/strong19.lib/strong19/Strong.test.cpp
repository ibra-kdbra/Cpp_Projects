#include "Strong.h"

#include "Strong.ostream.h"
#include "Strong.traits.h"

#include <gtest/gtest.h>
#include <type_traits>

using namespace strong19;
using meta19::TypePack;

DEFINE_STRONG(Position, int);
DEFINE_STRONG(Distance, int);

// should create static_assert error
// DEFINE_STRONG(Nested, Position);

// you only want to define these for tesing
DEFINE_STRONG_OSTREAM(Position);
DEFINE_STRONG_OSTREAM(Distance);

constexpr inline auto operator+(Position p, Distance d) -> Position { return Position{p.v + d.v}; }

TEST(Strong, usage) {
    auto p = Position{};
    EXPECT_EQ(p.v, 0);
    EXPECT_EQ(p, Position{});

    auto d = Distance{1};
    EXPECT_EQ(d.v, 1);
    EXPECT_EQ(d, Distance{1});

    auto p2 = p + d;
    EXPECT_EQ(p2, Position{1});

    // should not compile!
    // EXPECT_EQ(p, d);

    // show that ostream works for failing test
    // EXPECT_EQ(p2, Position{2});
}

TEST(Strong, ostream) {
    auto ss = std::stringstream{};
    ss << Position{1};
    EXPECT_EQ(ss.str(), "Position{1}");
}

DEFINE_STRONG(Velocity, double, struct MeterTag);

TEST(Strong, tags) {
    static_assert(std::is_same_v<StrongTags<Velocity>, TypePack<MeterTag>>);
    static_assert(std::is_same_v<StrongValueType<Velocity>, double>);
    static_assert(has_strong_tags<Velocity, MeterTag>);
    static_assert(has_strong_tags<Velocity, MeterTag>);
    static_assert(!has_strong_tags<int, MeterTag>);
}
