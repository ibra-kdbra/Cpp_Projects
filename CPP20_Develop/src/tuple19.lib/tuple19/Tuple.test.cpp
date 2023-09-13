#include "Tuple.h"

#include "Tuple.ostream.h"
#include "meta19/Index.h"

#include <gtest/gtest.h>
#include <stddef.h> // size_t
#include <utility>

using namespace tuple19;

namespace {
template<typename Type> constexpr size_t alignment(size_t offset = 0, size_t current = 0) {
    return current >= offset ? current : alignment<Type>(offset, current + alignof(Type));
}
} // namespace

TEST(Tuple, basic) {
    using T = Tuple<char, int, double>;

    static_assert(T::has_type<char>);
    static_assert(T::has_type<int>);
    static_assert(T::has_type<double>);
    static_assert(!T::has_type<float>);
    static_assert(!T::has_type<bool>);
    static_assert(!T::has_type<unsigned int>);

    static_assert(T::index_of<char> == 0);
    static_assert(T::index_of<int> == 1);
    static_assert(T::index_of<double> == 2);

    using meta19::_index;
    static_assert(std::is_same_v<const char&, decltype(std::declval<T>().at(_index<0>))>);
}

TEST(Tuple, access) {
    using meta19::_index;
    using meta19::type;

    using T = Tuple<char, int, double>;
    auto t = T{};

    t.amendAt(_index<1>) = 23;
    t.amendOf<double>() = 4.2;
    EXPECT_EQ((t.of(type<int>)), 23);
    EXPECT_EQ((t.at(_index<2>)), 4.2);
}

TEST(Tuple, copy_move) {
    using MultiArg = Tuple<char, int, double>;
    auto m0 = MultiArg{};
    auto m1 = m0; // copy
    auto m2 = std::move(m1); // move copy

    (void)m2;
    //    EXPECT_EQ(m0, m2); // uninitialized!
}
TEST(Tuple, construct_empty) {
    using T = Tuple<>;
    auto t = T{};
    t.visitAll([]() {});
}
TEST(Tuple, construct_single) {
    using SingleArg = Tuple<int>;

    auto s0 = SingleArg{23};
    SingleArg s1(s0); // Test that it won't use the argument forward constructor

    EXPECT_EQ(s0.at<0>(), s1.at<0>());
    EXPECT_EQ(s0, s1);
}
TEST(Tuple, construct_two) {
    using TwoArgs = Tuple<int, float>;
    auto s0 = TwoArgs{1, 2.3f};
    auto s1 = TwoArgs{s0};

    EXPECT_EQ(s0.at<0>(), s1.at<0>());
    EXPECT_EQ(s0.at<1>(), s1.at<1>());
}
TEST(Tuple, construct_copy) {
    using SingleArg = Tuple<int>;

    auto v = 23;
    auto s0 = SingleArg{v};
    SingleArg s1(s0); // Test that it won't use the argument forward constructor

    EXPECT_EQ(s0.at<0>(), s1.at<0>());
}

TEST(Tuple, ambigious) {
    using Ambiguous = Tuple<char, int, int>;
    auto a0 = Ambiguous{};
    auto a1 = Ambiguous::fromArgs('c');
    // auto a1b = Ambiguous::fromArgs('c', 23, 32); // not allowed! - arguments are treated out of order!
    auto a2 = Ambiguous{'c', 23, 32};

    (void)(a0);
    (void)(a1);
    EXPECT_EQ((a2.at<1>()), 23);
    EXPECT_EQ((a2.at<2>()), 32);
}

TEST(Tuple, fromTuple) {
    using Small = Tuple<char, int, double>;
    using Large = Tuple<int, double, char, float>;
    auto s = Small{'c', 23, 4.2};
    auto l = Large::fromTuple(s);

    EXPECT_EQ((l.of<char>()), 'c');
    EXPECT_EQ((l.of<double>()), 4.2);
    EXPECT_EQ((l.of<int>()), 23);
    EXPECT_EQ((l.of<float>()), 0);
}

TEST(Tuple, ostream) {
    auto out = std::stringstream{};
    using T = Tuple<char, int, double>;
    out << T{'c', 23, 4.2};
    EXPECT_EQ(out.str(), "[c; 23; 4.2]");
}
