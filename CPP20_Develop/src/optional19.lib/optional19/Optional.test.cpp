#include "Optional.h"

#include "Optional.equals.h"
#include "Optional.ostream.h"

#include <gtest/gtest.h>

using namespace optional19;

TEST(Optional, int) {
    using OptInt = Optional<int>;

    static_assert(sizeof(OptInt) > sizeof(int));
    {
        constexpr auto optInt = OptInt{};
        ASSERT_FALSE(optInt);
        ASSERT_EQ(optInt, OptInt{});

        ASSERT_EQ((optInt || 42), 42);

        constexpr const auto constOpt = optInt;
        ASSERT_FALSE(constOpt);
        ASSERT_EQ(constOpt || 42, 42);

        ASSERT_FALSE(constOpt && [](int v) { return v > 42; });
    }
    {
        constexpr auto optInt = OptInt{23};
        constexpr const auto constOpt = optInt;
        ASSERT_TRUE(optInt);
        ASSERT_EQ(constOpt, OptInt{23});

        ASSERT_EQ(optInt.value(), 23);
        ASSERT_EQ(optInt || 42, 23);

        auto l = constOpt.map([](int v) { return v * 2; });
        ASSERT_EQ(l, 23 * 2);

        ASSERT_FALSE(constOpt && [](int v) { return v > 42; });
        ASSERT_TRUE(constOpt && [](int v) { return v > 0; });

        if (optInt && constOpt) {
            // compiles?
        }
        if (optInt || constOpt) {
            // compiles?
        }
    }

    auto optInt = OptInt{};
    ASSERT_NE(optInt, OptInt{23});
    optInt = 23;
    ASSERT_EQ(optInt, OptInt{23});
}

struct Point {
    float x{}, y{};
    constexpr bool operator==(const Point&) const = default;
};
auto operator<<(std::ostream& out, const Point& point) -> std::ostream& {
    return out << "Point{x:" << point.x << ",y:" << point.y << "}";
}

TEST(Optional, Point) {
    static_assert(!(std::is_trivially_default_constructible_v<Point> && std::is_trivially_destructible_v<Point>));

    using OptPoint = Optional<Point>;
    static_assert(sizeof(OptPoint) > sizeof(Point));
    {
        auto optPoint = OptPoint{};
        ASSERT_FALSE(optPoint);
        ASSERT_EQ(optPoint, OptPoint{});

        ASSERT_EQ((optPoint || Point{3.f, 4.f}), (Point{3.f, 4.f}));

        optPoint.emplace(3.f, 4.f);
        ASSERT_TRUE(optPoint);
        ASSERT_EQ(optPoint, (OptPoint{inplace, 3.f, 4.f}));
    }
}
