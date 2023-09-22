#include "Tuple.h"

#include "Tuple.bind.h"
#include "Tuple.ostream.h"

#include <gtest/gtest.h>

using namespace tuple19;

TEST(TupleBind, get) {
    using meta19::_index;
    using meta19::type;

    using T = Tuple<char, int, double>;
    auto t = T{'x', 23, 4.2};

    EXPECT_EQ(get<char>(t), 'x');
    EXPECT_EQ(get<int>(t), 23);
    EXPECT_EQ(get<double>(t), 4.2);
}

TEST(TupleBind, copy) {
    auto tpl = Tuple{12, 'a'};
    auto [num, chr] = tpl;

    EXPECT_EQ(num, 12);
    EXPECT_EQ(chr, 'a');

    num = 23;
    EXPECT_EQ(tpl, (Tuple{12, 'a'}));
}

TEST(TupleBind, reference) {
    auto tpl = Tuple{12, 'a'};
    auto& [num, chr] = tpl;

    EXPECT_EQ(num, 12);
    EXPECT_EQ(chr, 'a');

    num = 23;
    EXPECT_EQ(tpl, (Tuple{23, 'a'}));
}

TEST(TupleBind, moved) {
    auto&& [num, chr] = Tuple{12, 'a'};

    EXPECT_EQ(num, 12);
    EXPECT_EQ(chr, 'a');
}
