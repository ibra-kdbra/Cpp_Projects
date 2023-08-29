#include "ReadArchive.h"
#include "dynamicWrite.h"
#include "serialize.std_vector.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(serialize, std_vector) {
    using T = std::vector<int>;
    auto input = T{7, 13, 23};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, std_vector_empty) {
    using T = std::vector<int>;
    auto input = T{};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}

TEST(serialize, std_vector_bool) {
    using T = std::vector<bool>;
    auto input = T{1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
