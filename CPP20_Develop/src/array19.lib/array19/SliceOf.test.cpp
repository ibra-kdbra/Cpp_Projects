#include "SliceOf.h"

#include "SliceOf.carray.h"
#include "SliceOf.store.h"

#include <gtest/gtest.h>

using namespace array19;

void constexpr_SliceOf_test() {
    constexpr static int a[3] = {1, 2, 3};
    constexpr auto s = sliceOfCArray(a);

    static_assert(s.count() == 3);
    static_assert(s[1] == 2);

    constexpr auto sum = [&] {
        auto r = 0;
        for (auto v : s) r += v;
        return r;
    }();
    static_assert(sum == 6);
}

TEST(SliceOf, Access) {
    int a[3] = {1, 2, 3};
    auto s = sliceOfCArray(a);

    EXPECT_EQ(s.count(), 3u);
    EXPECT_EQ(s[1], 2);

    auto sum = 0;
    for (auto v : s) sum += v;
    EXPECT_EQ(sum, 6);
}

void mutable_SliceOf_test() {
    constexpr auto sa = [] {
        int a[] = {1, 2, 3};
        auto s = amendSliceOfCArray(a);
        s[1] = 5;
        return storeSlice<3>(s);
    }();

    static_assert(sa[1] == 5);
}

TEST(SliceOf, Mutate) {
    int a[] = {1, 2, 3, 4};
    auto s = amendSliceOfCArray(a);
    s[1] = 5;
    EXPECT_EQ(a[1], 5);

    auto sa = storeSlice<4>(s);
    EXPECT_EQ(sa[1], 5);
    EXPECT_EQ(sa[3], 4);
}
