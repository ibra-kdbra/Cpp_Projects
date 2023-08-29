#include "ReadArchive.h"
#include "dynamicWrite.h"
#include "serialize.std_string.h"
#include "serialize.std_tuple.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(serialize, std_tuple) {
    using T = std::tuple<int, float, std::string>;
    auto input = T{23, 3.14f, std::string("glurbz")};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
