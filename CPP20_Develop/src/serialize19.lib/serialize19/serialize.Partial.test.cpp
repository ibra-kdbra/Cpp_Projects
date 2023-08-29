#include <gtest/gtest.h>
#include <partial19/Partial.equals.h>
#include <partial19/Partial.ostream.h>
#include <serialize19/ReadArchive.h>
#include <serialize19/dynamicWrite.h>
#include <serialize19/serialize.Partial.h>
#include <serialize19/serialize.std_string.h>

using partial19::Partial;
using serialize19::dynamicWrite;
using serialize19::ReadArchive;

TEST(serialize, Partial) {
    using T = Partial<int, float, std::string>;
    auto input = T::fromArgs(23);

    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = T{};
    serialize(reader, output);

    EXPECT_EQ(input, output);
}
