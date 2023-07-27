#include "AllocatedArrayOf.h"

#include "AllocatedArrayOf.equals.h"
#include "AllocatedArrayOf.ostream.h"
#include "SliceOf.carray.h"
#include "SliceOf.equals.h"
#include "SliceOf.ostream.h"

#include <gtest/gtest.h>
#include <utility>

using namespace array19;

TEST(AllocatedArrayOf, construct) {
    auto v = AllocatedArrayOf{1, 2, 3};
    ASSERT_EQ(sliceOfCArray({1, 2, 3}), SliceOf{v});
}

namespace {

struct NonTrivial;
using NonTrivialArray = AllocatedArrayOf<NonTrivial>;

struct User {
    NonTrivialArray v;

    User() = default;
};

struct NonTrivial {
    NonTrivial() : v(1) {}
    explicit NonTrivial(int v) : v(v) {}
    NonTrivial(const NonTrivial& o) : v(o.v) {}
    NonTrivial& operator=(const NonTrivial& o) { return v = o.v, *this; }
    NonTrivial(NonTrivial&& o) noexcept : v(o.v) {}
    NonTrivial& operator=(NonTrivial&& o) noexcept { return v = o.v, *this; }
    ~NonTrivial() noexcept { c = false; }
    bool operator==(const NonTrivial&) const = default;

    int value() const { return v; }

private:
    bool c{true};
    int v;
};

[[maybe_unused]] auto operator<<(std::ostream& out, const NonTrivial& c) -> std::ostream& { return out << c.value(); }

} // namespace

TEST(AllocatedArrayOf, NontrivialExample) {
    auto v = AllocatedArrayOf<NonTrivial>{};

    EXPECT_TRUE(v.isEmpty());
    ASSERT_EQ(v.count(), 0u);

    v = AllocatedArrayOf{NonTrivial{}, NonTrivial{2}};

    ASSERT_EQ(v.count(), 2u);

    auto v2 = v;
    ASSERT_EQ(v, v2);
}

TEST(AllocatedArrayOf, MoveAssignAfterMove) {
    using E = NonTrivial;
    using AA = AllocatedArrayOf<E>;
    auto v = AA{E{1}};

    auto v2 = std::move(v);
    ASSERT_EQ(v2, (AA{E{1}}));

    v = AA{E{2}, E{3}};
    ASSERT_EQ(v, (AA{E{2}, E{3}}));
}

TEST(AllocatedArrayOf, CopyAssignAfterMove) {
    using E = NonTrivial;
    using AA = AllocatedArrayOf<E>;
    auto v = AA{E{1}};

    auto v2 = std::move(v);
    ASSERT_EQ(v2, (AA{E{1}}));

    auto v3 = AA{E{2}, E{3}};
    v = v3;
    ASSERT_EQ(v, (AA{E{2}, E{3}}));
}
