#include <array19/DynamicArrayOf.equals.h>
#include <array19/DynamicArrayOf.ostream.h>
#include <gtest/gtest.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.DynamicArrayOf.h>

using namespace serialize19;

TEST(serialize, DynamicArrayOf) {
    using T = array19::DynamicArrayOf<int>;
    auto input = T{7, 13, 23};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, DynamicArrayOf_empty) {
    using T = array19::DynamicArrayOf<int>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
