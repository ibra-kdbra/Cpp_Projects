#include <gtest/gtest.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.Tuple.h>
#include <serialize19/serialize.std_string.h>
#include <tuple19/Tuple.ostream.h>

using serialize19::dynamicWrite;
using serialize19::ReadArchive;
using tuple19::Tuple;

TEST(serialize, Tuple) {
    using T = Tuple<int, float, std::string>;
    auto input = T{23, 3.14f, std::string("baz")};

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(output, input);
}
