#include <gtest/gtest.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.Variant.h>
#include <serialize19/serialize.std_string.h>
#include <variant19/Variant.equals.h>
#include <variant19/Variant.ostream.h>

using serialize19::dynamicWrite;
using serialize19::ReadArchive;
using variant19::Variant;

TEST(serialize, Variant) {
    using T = Variant<int, float, std::string>;
    const auto refval = std::string("glurbz");
    auto input = T{refval};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
