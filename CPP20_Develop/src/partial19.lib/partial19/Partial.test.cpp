#include "Partial.h"

#include "Partial.equals.h"
#include "Partial.ostream.h"

#include <gtest/gtest.h>
#include <stddef.h> // size_t
#include <utility>

using namespace partial19;

using meta19::type;

TEST(Partial, empty) {
    using P = Partial<char, int, float>;
    auto p0 = P{};
    ASSERT_FALSE(p0.which()[0]);
    ASSERT_FALSE(p0.which()[1]);
    ASSERT_FALSE(p0.which()[2]);
}

void callMe(Partial<char, int, float>) {}

TEST(Partial, construction) {
    using P = Partial<char, int, float>;
    auto p1 = P::fromArgs('c', 2.3f);
    auto p2 = p1; // copy constructor
    auto p4 = p1; // preparation for move construct
    auto p3 = P{std::move(p4)}; // move construct

    EXPECT_EQ(p1.at<0>(), 'c');
    EXPECT_EQ(p1.at<2>(), 2.3f);
    EXPECT_EQ(p1, p3);
    EXPECT_EQ(p1, p2);
    EXPECT_EQ(p2, p3);

    callMe(p2);
}

TEST(Partial, assignment) {
    using P = Partial<char, int, float>;
    auto p0 = P::fromArgs('c', 2.3f);
    auto p1 = P{};
    p1 = p0; // copy operator
    EXPECT_EQ(p0, p1);
    auto p2 = P{};
    EXPECT_EQ(p2, P{});
    p2 = std::move(p1); // move operator

    EXPECT_EQ(p0.at<0>(), 'c');
    EXPECT_EQ(p0.at<2>(), 2.3f);
    EXPECT_EQ(p0, p2);
}

TEST(Partial, fromFactory) {
    using P = Partial<char, int, float>;
    const auto p = P::fromFactory(
        [](size_t i) { return (0 == i % 2); },
        []<class T>(Type<T>*, void* ptr) {
            if constexpr (std::is_same_v<T, char>) {
                new (ptr) char('a');
            }
            if constexpr (std::is_same_v<T, int>) {
                new (ptr) int(23);
            }
            if constexpr (std::is_same_v<T, float>) {
                new (ptr) float(3.14f);
            }
        });
    ASSERT_TRUE(p.which()[0]);
    ASSERT_FALSE(p.which()[1]);
    ASSERT_TRUE(p.which()[2]);

    EXPECT_EQ(p.at<0>(), 'a');
    EXPECT_EQ(p.at<2>(), 3.14f);
}

TEST(Partial, type) {
    using P = Partial<char, int, float>;
    auto p = P::fromArgs('\x23', 23);
    ASSERT_TRUE(p.which().of(type<char>));
    ASSERT_TRUE(p.which()[0]);
    EXPECT_EQ(0x23, p.of(type<char>));
    EXPECT_EQ(0x23, p.at(_index<0>));
    ASSERT_TRUE(p.which().of(type<int>));
    ASSERT_TRUE(p.which()[1]);
    ASSERT_EQ(23, p.of(type<int>));
    EXPECT_EQ(23, p.at(_index<1>));
}

// note: this would only store one int, due to implementation design!
// TEST(Partial, with_duplicated_types) {
//     using P = Partial<int, int>;
//     auto p = P::fromArgs(23, 32);
//     ASSERT_TRUE(p.which().of(type<int>));
//     ASSERT_EQ(32, p.of(type<int>)); // note: ambigious!

//     EXPECT_EQ(23, p.at(_index<0>));
//     EXPECT_EQ(32, p.at(_index<1>));
// }

TEST(Partial, visitInitialized) {
    using P = Partial<char, int, float>;
    auto p = P::fromArgs('\x23', 2.3f);

    size_t count = 0;
    p.visitInitialized([&]<class V>(const V&) {
        ++count;
        EXPECT_TRUE((std::is_same_v<V, char> || std::is_same_v<V, float>));
    });
    EXPECT_EQ(count, 2u);
}

TEST(Partial, visitWithIndex) {
    using P = Partial<char, int, float>;
    auto p = P::fromArgs('\x23', 2.3f);

    size_t count = 0;
    p.visitWithIndex([&]<size_t I, class V>(Index<I>*, const V&) {
        ++count;
        EXPECT_TRUE(I == 0 || I == 2);
        EXPECT_TRUE((std::is_same_v<V, char> || std::is_same_v<V, float>));
    });
    EXPECT_EQ(count, 2u);
}

TEST(Partial, ostream) {
    auto out = std::stringstream{};
    using P = Partial<char, int, float>;
    out << P::fromArgs('a', 2.3f);
    EXPECT_EQ(out.str(), "<[a; 2.3]>");
}

// note: merge often contains custom functionality
// note: We keep the test implementation to show how it works.
template<class... Ts> auto merge(const Partial<Ts...>& a, const Partial<Ts...>& b) -> Partial<Ts...> {
    using P = Partial<Ts...>;
    return P::fromFactory(
        [&](size_t i) { return a.which()[i] || b.which()[i]; },
        [&]<class T>(Type<T>*, void* ptr) { new (ptr) T((b.which().of(type<T>) ? b : a).template of<T>()); });
}

TEST(Partial, disjunct_merge) {
    using P = Partial<char, int, float>;
    auto p0 = P::fromArgs('\x23');
    auto p1 = P::fromArgs(2.3f);
    EXPECT_EQ(merge(p0, p1), P::fromArgs('\x23', 2.3f));
}

TEST(Partial, overwrite_merge) {
    using P = Partial<char, int, float>;
    auto p0 = P::fromArgs(2);
    auto p1 = P::fromArgs(4);
    EXPECT_EQ(merge(p0, p1), P::fromArgs(4));
    EXPECT_EQ(merge(p1, p0), P::fromArgs(2));
}
