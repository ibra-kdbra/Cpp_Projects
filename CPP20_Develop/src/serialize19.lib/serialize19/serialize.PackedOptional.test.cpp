#include <gtest/gtest.h>
#include <optional19/PackedOptional.equals.h>
#include <optional19/PackedOptional.h>
#include <optional19/PackedOptional.ostream.h>
#include <optional19/PackedOptionalNan.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.PackedOptional.h>

using namespace serialize19;
using optional19::defaultOf;
using optional19::PackedOptional;
using optional19::PackedOptionalNan;

TEST(serialize, PackedOptional_value) {
    using T = PackedOptional<defaultOf<int>>;
    auto input = T{23};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), sizeof(int)); // contains just the int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, PackedOptional_default) {
    using T = PackedOptional<defaultOf<int>>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), sizeof(int)); // contains just the int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, PackedOptionalNan_value) {
    using T = PackedOptionalNan<double>;
    auto input = T{2.3};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), sizeof(double)); // contains just the int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, PackedOptionalNan_default) {
    using T = PackedOptionalNan<double>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), sizeof(double)); // contains just the int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
