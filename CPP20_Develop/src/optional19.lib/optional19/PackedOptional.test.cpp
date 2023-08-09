#include "PackedOptional.h"

#include "PackedOptional.equals.h"
#include "PackedOptional.ostream.h"

#include <gtest/gtest.h>

using namespace optional19;

TEST(PackedOptional, defaultOfInt) {
    using OptInt = PackedOptional<defaultOf<int>>;

    static_assert(sizeof(OptInt) == sizeof(int));
    {
        constexpr auto optInt = OptInt{};
        ASSERT_FALSE(optInt);
        ASSERT_EQ(optInt, OptInt{});

        ASSERT_EQ((optInt || 42), 42);

        constexpr const auto constOpt = optInt;
        ASSERT_FALSE(constOpt);
        ASSERT_EQ((constOpt || 42), 42);

        ASSERT_FALSE(constOpt && [](int v) { return v > 42; });
    }
    {
        constexpr auto optInt = OptInt{23};
        constexpr const auto constOpt = optInt;
        ASSERT_TRUE(optInt);
        // ASSERT_EQ(constOpt, 23);

        ASSERT_EQ(optInt.value(), 23);
        ASSERT_EQ((optInt || 42), 23);

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

constexpr auto intInvalid() { return -1; }

TEST(PackedOptional, invalidFunc) {
    using OptInt = PackedOptional<intInvalid>;

    static_assert(sizeof(OptInt) == sizeof(int));
    {
        constexpr auto optInt = OptInt{};
        static_assert(!optInt);
        static_assert(optInt == OptInt{});
        static_assert(optInt.value() == intInvalid());

        static_assert((optInt || 42) == 42);

        constexpr const auto constOpt = optInt;
        static_assert(!constOpt);
        static_assert((constOpt || 42) == 42);

        static_assert(!(constOpt && [](int v) { return v > 42; }));
    }
    {
        constexpr auto optInt = OptInt{23};
        static_assert(optInt);
        static_assert(optInt == OptInt{23});

        static_assert(optInt.value() == 23);
        static_assert((optInt || 42) == 23);

        constexpr const auto constOpt = optInt;
        constexpr auto l = constOpt.map([](int v) { return v * 2; });
        static_assert(l == 23 * 2);

        static_assert(!(constOpt && [](int v) { return v > 42; }));
        static_assert(constOpt && [](int v) { return v > 0; });
    }

    auto optInt = OptInt{};
    ASSERT_NE(optInt, OptInt{23});
    optInt = 23;
    ASSERT_EQ(optInt, OptInt{23});
}

TEST(PackedOptional, pointer) {
    using OptPtr = PackedOptional<nullptrTo<int>>;

    static_assert(sizeof(OptPtr) == sizeof(int*));
}
