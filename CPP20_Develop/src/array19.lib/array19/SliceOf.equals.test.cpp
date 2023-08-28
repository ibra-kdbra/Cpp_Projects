#include "SliceOf.carray.h"
#include "SliceOf.equals.h"

using namespace array19;

void constexpr_SliceOf_equals_test() {
    constexpr static int a[3] = {1, 2, 3};
    constexpr auto as = sliceOfCArray(a);

    constexpr static int b[3] = {1, 2, 3};
    constexpr auto bs = amendSliceOfCArray(b);

    static_assert(as == bs);
}

void constexpr_SliceOf_equals_empty_test() {
    constexpr auto as = SliceOf<int>{};
    constexpr auto bs = SliceOf<int>{};
    static_assert(as == bs);
}

void constexpr_SliceOf_equals_count_differ_test() {
    constexpr static int a[3] = {1, 2, 3};
    constexpr auto as = sliceOfCArray(a);

    constexpr auto bs = SliceOf<const int>{};

    static_assert(as != bs);
}

void constexpr_SliceOf_equals_value_differ_test() {
    constexpr static int a[3] = {1, 2, 3};
    constexpr auto as = sliceOfCArray(a);

    constexpr static int b[3] = {1, 99, 3};
    constexpr auto bs = amendSliceOfCArray(b);

    static_assert(as != bs);
}
