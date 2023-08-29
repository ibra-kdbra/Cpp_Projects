#include "ReadArchive.h"
#include "dynamicWrite.h"
#include "serialize.std_bitset.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(serialize, std_bitset23) {
    using T = std::bitset<23>;
    const auto refval = uint64_t{0x70'0f'fe};
    auto input = T{refval};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output.to_ullong(), refval);
}

TEST(serialize, std_bitset24) {
    using T = std::bitset<24>;
    const auto refval = uint64_t{0xf0'0f'fe};
    auto input = T{refval};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output.to_ullong(), refval);
}
