#include "DynamicArrayOf.h"

#include "Array.h"
#include "DynamicArrayOf.equals.h"
#include "DynamicArrayOf.ostream.h"
#include "SliceOf.carray.h"
#include "SliceOf.equals.h"

#include <gtest/gtest.h>
#include <sstream>
#include <utility>

using namespace array19;

TEST(DynamicArrayOf, intExample) {

    auto v = DynamicArrayOf<int>{};

    EXPECT_TRUE(v.isEmpty());
    EXPECT_EQ(v.totalCapacity(), 0u);
    EXPECT_EQ(v.unusedCapacity(), 0u);
    ASSERT_EQ(v.count(), 0u);

    v.ensureUnusedCapacity(1);

    EXPECT_GE(v.totalCapacity(), 1u);
    EXPECT_GE(v.unusedCapacity(), 1u);
    ASSERT_EQ(v.count(), 0u);

    v.append(sliceOfCArray({12, 23}));

    ASSERT_EQ(v.count(), 2u);
    EXPECT_GE(v.totalCapacity(), 2u);

    v.append(sliceOfCArray({42, 64}));

    ASSERT_EQ(v.count(), 4u);

    v.splice(v.amendBegin() + 1, 2, sliceOfCArray({99}));

    auto v2 = DynamicArrayOf<int>{};
    v2.append(sliceOfCArray({12, 99, 64}));
    ASSERT_EQ(v, v2);

    v2.append(sliceOfCArray({45}));
    v2 = v; // copy
    ASSERT_EQ(v, v2);

    v.append(sliceOfCArray({45}));
    v2 = std::move(v);
    ASSERT_EQ(sliceOfCArray({12, 99, 64, 45}), SliceOf{v2});
}

TEST(DynamicArrayOf, construct) {
    auto v = DynamicArrayOf{1, 2, 3};
    ASSERT_EQ(sliceOfCArray({1, 2, 3}), SliceOf{v});
}

namespace {

struct NonTrivial;
using NonTrivialArray = DynamicArrayOf<NonTrivial>;

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

TEST(DynamicArrayOf, NontrivialExample) {
    auto v = DynamicArrayOf<NonTrivial>{};

    EXPECT_TRUE(v.isEmpty());
    EXPECT_EQ(v.totalCapacity(), 0u);
    EXPECT_EQ(v.unusedCapacity(), 0u);
    ASSERT_EQ(v.count(), 0u);

    v.ensureUnusedCapacity(1u);

    EXPECT_GE(v.totalCapacity(), 1u);
    EXPECT_GE(v.unusedCapacity(), 1u);
    ASSERT_EQ(v.count(), 0u);

    v.append(sliceOfCArray({NonTrivial{12}, NonTrivial{23}}));

    ASSERT_EQ(v.count(), 2u);
    EXPECT_GE(v.totalCapacity(), 2u);

    v.append(sliceOfCArray({NonTrivial{42}, NonTrivial{64}}));

    ASSERT_EQ(v.count(), 4u);

    v.splice(v.amendBegin() + 1, 2, sliceOfCArray({NonTrivial{99}}));

    auto v2 = DynamicArrayOf<NonTrivial>{};
    v2.append(sliceOfCArray({NonTrivial{12}, NonTrivial{99}, NonTrivial{64}}));
    ASSERT_EQ(v, v2);
}

TEST(DynamicArrayOf, MoveAssignAfterMove) {
    using E = NonTrivial;
    using AA = DynamicArrayOf<E>;
    auto v = AA{E{1}};

    auto v2 = std::move(v);
    ASSERT_EQ(v2, (AA{E{1}}));

    v = AA{E{2}, E{3}};
    ASSERT_EQ(v, (AA{E{2}, E{3}}));
}

TEST(DynamicArrayOf, CopyAssignAfterMove) {
    using E = NonTrivial;
    using AA = DynamicArrayOf<E>;
    auto v = AA{E{1}};

    auto v2 = std::move(v);
    ASSERT_EQ(v2, (AA{E{1}}));

    auto v3 = AA{E{2}, E{3}};
    v = v3;
    ASSERT_EQ(v, (AA{E{2}, E{3}}));
}

namespace {

struct Vec3 {
    double x, y, z;

    bool operator==(const Vec3&) const = default;

    [[maybe_unused]] friend auto operator<<(std::ostream& o, const Vec3& v) -> std::ostream& {
        return o << "x:" << v.x << ", y:" << v.y << ", z:" << v.z;
    }
};

} // namespace

TEST(DynamicArrayOf, EmplaceStruct) {
    auto v = DynamicArrayOf<Vec3>{};
    v.emplace_back(1.1, 2.2, 3.3);

    auto v2 = DynamicArrayOf{Vec3{1.1, 2.2, 3.3}};
    EXPECT_EQ(v, v2);
}

namespace {

struct Recorder {
    static inline std::stringstream store;
    int v{};

    Recorder(int v) : v(v) { store << v << ".ctor\n"; }

    Recorder() { store << "{}.ctor\n"; }
    ~Recorder() noexcept { store << v << ".dtor\n"; }
    Recorder(const Recorder& o) : v(100 + o.v) { store << v << ".ctor(const&)\n"; }
    Recorder& operator=(const Recorder& o) {
        return (store << v << ".op=(const& " << (100 + o.v) << ")\n"), v = 100 + o.v, *this;
    }
    Recorder(Recorder&& o) noexcept : v(o.v) { o.v = 1000 + v, store << v << ".ctor(&&)\n"; }
    Recorder& operator=(Recorder&& o) noexcept {
        return (store << v << ".op=(&& " << o.v << ")\n"), v = o.v, o.v = 1000 + v, *this;
    }

    bool operator==(const Recorder&) const = default;

    [[maybe_unused]] friend auto operator<<(std::ostream& o, const Recorder& r) -> std::ostream& { return o << r.v; }
};

} // namespace

TEST(DynamicArrayOf, RecordedCopyAndMove) {
    using T = DynamicArrayOf<Recorder>;
    auto v2 = T{};
    {
        Recorder::store = std::stringstream{};

        auto v = T{};
        v.ensureCapacity(5);
        ASSERT_EQ(v.unusedCapacity(), 5u); // 5 is currently the minimum

        EXPECT_EQ(Recorder::store.str(), "") << "No elements are constructed yet";

        Recorder::store = std::stringstream{};
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        v.emplace_back(4);
        v.emplace_back(5);

        EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
4.ctor
5.ctor
)") << "5 elements constructed";

        Recorder::store = std::stringstream{};
        ASSERT_EQ(v.unusedCapacity(), 0u) << "all capacity used";
        v.emplace_back(6);
        ASSERT_NE(v.unusedCapacity(), 0u) << "capacity grows by more than one";

        EXPECT_EQ(Recorder::store.str(), R"(1.ctor(&&)
2.ctor(&&)
3.ctor(&&)
4.ctor(&&)
5.ctor(&&)
1001.dtor
1002.dtor
1003.dtor
1004.dtor
1005.dtor
6.ctor
)") << "moved 5 elements to new store, destructed old store, constructed new element";

        Recorder::store = std::stringstream{};
        v2 = std::move(v);
        ASSERT_EQ(Recorder::store.str(), "") << "move of array does not move elements";
    }
    ASSERT_EQ(Recorder::store.str(), "") << "destruct of moved from array has no influence";
    {
        auto v = v2;
        EXPECT_EQ(Recorder::store.str(), R"(101.ctor(const&)
102.ctor(const&)
103.ctor(const&)
104.ctor(const&)
105.ctor(const&)
106.ctor(const&)
)") << "copy of array copied all 6 elements";

        Recorder::store = std::stringstream{};
    }
    EXPECT_EQ(Recorder::store.str(), R"(101.dtor
102.dtor
103.dtor
104.dtor
105.dtor
106.dtor
)") << "destruction of copy destroys copied elements";

    {
        Recorder::store = std::stringstream{};

        auto v = T{Array{Recorder{1}, 2, 3}.move()};
        EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
1.ctor(&&)
2.ctor(&&)
3.ctor(&&)
1003.dtor
1002.dtor
1001.dtor
)") << "construct static array, move to dynamic array, destroy static array";
    }
}

TEST(DynamicArrayOf, RecordedSpliceGrowOverCapacity) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{1, 2, 3, 4, 5};
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
4.ctor
5.ctor
)") << "5 array elements constructed";

    Recorder::store = std::stringstream{};
    auto insert = Array{Recorder{12}, 13};
    EXPECT_EQ(Recorder::store.str(), R"(12.ctor
13.ctor
)") << "2 array elements constructed";

    Recorder::store = std::stringstream{};
    ASSERT_EQ(v.unusedCapacity(), 0u) << "all capacity used";
    v.splice(v.amendBegin() + 1, 1, insert);
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor(&&)
112.ctor(const&)
113.ctor(const&)
3.ctor(&&)
4.ctor(&&)
5.ctor(&&)
1001.dtor
2.dtor
1003.dtor
1004.dtor
1005.dtor
)") << "1,3,4,5 is moved to new location; 12,13 are copied between; old location is destructed";
    EXPECT_EQ(v, (T{1, 112, 113, 3, 4, 5}));
}

TEST(DynamicArrayOf, RecordedSpliceShrink) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{1, 2, 3, 4, 5};
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
4.ctor
5.ctor
)") << "5 array elements constructed";

    Recorder::store = std::stringstream{};
    auto insert = Array{Recorder{12}, 13};
    EXPECT_EQ(Recorder::store.str(), R"(12.ctor
13.ctor
)") << "2 array elements constructed";

    Recorder::store = std::stringstream{};
    v.splice(v.amendBegin() + 1, 3, insert);
    EXPECT_EQ(Recorder::store.str(), R"(2.op=(const& 112)
3.op=(const& 113)
4.op=(&& 5)
1005.dtor
)");

    EXPECT_EQ(v, (T{1, 112, 113, 5}));
}

TEST(DynamicArrayOf, RecordedSpliceInsertOverEnd) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{};
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
)") << "3 array elements constructed";

    Recorder::store = std::stringstream{};
    auto insert = Array{Recorder{12}, 13, 14};
    EXPECT_EQ(Recorder::store.str(), R"(12.ctor
13.ctor
14.ctor
)") << "3 array elements constructed";

    Recorder::store = std::stringstream{};
    ASSERT_EQ(v.unusedCapacity(), insert.count - 1) << "all capacity used";
    v.splice(v.amendBegin() + 1, 1, insert);
    EXPECT_EQ(Recorder::store.str(), R"(3.ctor(&&)
2.op=(const& 112)
1003.op=(const& 113)
114.ctor(const&)
)");
    EXPECT_EQ(v, (T{1, 112, 113, 114, 3}));
}

TEST(DynamicArrayOf, RecordedSpliceInsertBeforeEnd) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{};
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    v.emplace_back(4);
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
4.ctor
)") << "4 array elements constructed";

    Recorder::store = std::stringstream{};
    auto insert = Array{Recorder{12}, 13};
    EXPECT_EQ(Recorder::store.str(), R"(12.ctor
13.ctor
)") << "2 array elements constructed";

    Recorder::store = std::stringstream{};
    ASSERT_EQ(v.unusedCapacity(), insert.count - 1) << "all capacity used";
    v.splice(v.amendBegin() + 1, 1, insert);
    EXPECT_EQ(Recorder::store.str(), R"(4.ctor(&&)
1004.op=(&& 3)
2.op=(const& 112)
1003.op=(const& 113)
)");
    EXPECT_EQ(v, (T{1, 112, 113, 3, 4}));
}

TEST(DynamicArrayOf, RecordedSpliceRemoveFirst) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{};
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
)") << "3 array elements constructed";

    Recorder::store = std::stringstream{};
    v.splice(v.amendBegin(), 1, {});
    EXPECT_EQ(Recorder::store.str(), R"(1.op=(&& 2)
1002.op=(&& 3)
1003.dtor
)");
    EXPECT_EQ(v, (T{2, 3}));
}

TEST(DynamicArrayOf, RecordedSpliceRemoveLast) {
    using T = DynamicArrayOf<Recorder>;

    Recorder::store = std::stringstream{};
    auto v = T{};
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    EXPECT_EQ(Recorder::store.str(), R"(1.ctor
2.ctor
3.ctor
)") << "3 array elements constructed";

    Recorder::store = std::stringstream{};
    v.splice(v.amendBegin() + 2, 1, {});
    EXPECT_EQ(Recorder::store.str(), R"(3.dtor
)");
    EXPECT_EQ(v, (T{1, 2}));
}
