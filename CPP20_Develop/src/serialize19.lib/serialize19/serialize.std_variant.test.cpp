#include "ReadArchive.h"
#include "dynamicWrite.h"
#include "serialize.std_string.h"
#include "serialize.std_variant.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(serialize, std_variant) {
    using T = std::variant<int, float, std::string>;
    const auto refval = std::string("glurbz");
    auto input = T{refval};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
