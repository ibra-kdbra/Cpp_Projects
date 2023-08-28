#include "Endianess.h"
#include "array19/Array.ostream.h"

#include <gtest/gtest.h>

using namespace serialize19;

TEST(EndianFlipFor, FourByteArray) {
    auto input = Array<uint8_t, 4>{1, 2, 3, 4};
    auto output = endianFlipFor(input);
    EXPECT_EQ(output, (Array<uint8_t, 4>{4, 3, 2, 1}));
}
TEST(EndianFlipFor, TenByteArray) {
    auto input = Array<uint8_t, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto output = endianFlipFor(input);
    EXPECT_EQ(output, (Array<uint8_t, 10>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
}
TEST(EndianFlipFor, u16) {
    auto input = uint16_t{0x0102};
    auto output = endianFlipFor<uint16_t>(input);
    EXPECT_EQ(output, 0x0201);
}
TEST(EndianFlipFor, u32) {
    auto input = uint32_t{0x01020304};
    auto output = endianFlipFor<uint32_t>(input);
    EXPECT_EQ(output, 0x04030201u);
}
TEST(EndianFlipFor, i32) {
    auto input = int32_t{0x01020384};
    auto output = endianFlipFor<int32_t>(input);
    EXPECT_EQ(output, static_cast<int32_t>(0x84030201));
}
TEST(EndianFlipFor, u64) {
    auto input = uint64_t{0x0102030405060708};
    auto output = endianFlipFor<uint64_t>(input);
    EXPECT_EQ(output, 0x0807060504030201ull);
}
