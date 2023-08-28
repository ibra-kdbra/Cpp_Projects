#include <gtest/gtest.h>
#include <optional19/Optional.equals.h>
#include <optional19/Optional.h>
#include <optional19/Optional.ostream.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.Optional.h>

using namespace serialize19;
using optional19::Optional;

TEST(serialize, Optional_value) {
    using T = Optional<int>;
    auto input = T{23};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), 1 + sizeof(int)); // contains just a bool + int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, Optional_empty) {
    using T = Optional<int>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), 1u); // contains just a bool

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
