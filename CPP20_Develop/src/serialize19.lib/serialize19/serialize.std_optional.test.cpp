#include "ReadArchive.h"
#include "dynamicWrite.h"
#include "serialize.std_optional.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(serialize, std_optional_value) {
    using T = std::optional<int>;
    auto input = T{23};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), 1 + sizeof(int)); // contains just a bool + int

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, std_optional_empty) {
    using T = std::optional<int>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    ASSERT_EQ(buffer.size(), 1u); // contains just a bool

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
