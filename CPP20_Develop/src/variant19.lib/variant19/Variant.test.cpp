#include "Variant.h"

#include "Variant.equals.h"
#include "Variant.ostream.h"
#include "meta19/Index.h"
#include "meta19/Type.h"

#include <gtest/gtest.h>
#include <utility>

using namespace variant19;
using meta19::_index;
using meta19::type;

TEST(Variant, construction) {
    using V = Variant<char, int, double>;

    auto v0 = V{};
    auto v1 = v0; // copy
    auto v2 = std::move(v1); // move copy
    EXPECT_EQ(v0, v2);

    //    // Inplace creation with type
    //    auto v3 = Variant<char, int, float>(type<int>, 23.14f);
    //    v3.visit([](auto value) {
    //        EXPECT_EQ(sizeof(value), sizeof(int));
    //        EXPECT_EQ(value, 23);
    //    });

    //    // Inplace creation with index
    //    auto v4 = Variant<char, int, float>(_index<1>, 23.14f);
    //    v4.visit([](auto value) {
    //        EXPECT_EQ(sizeof(value), sizeof(int));
    //        EXPECT_EQ(value, 23);
    //    });

    //    // Type without default
    //    struct S {
    //        S() = delete;
    //        S(int) {}
    //    };
    //    auto v5 = Variant<S>(type<S>, 2);
    //    v5.visit([](auto value) { EXPECT_EQ(sizeof(value), sizeof(S)); });
}

// TEST(Variant, emplace) {
//    using meta19::_index;
//    using V = Variant<char, int, float>;

//    // Inplace change of type with type
//    auto v0 = V{};
//    v0.emplace(type<int>, 23.14f);
//    v0.visit([](auto value) {
//        EXPECT_EQ(sizeof(value), sizeof(int));
//        EXPECT_EQ(value, 23);
//    });

//    // Inplace change of type with index
//    auto v1 = V{};
//    v1.emplace(_index<1>, 23.14f);
//    v1.visit([](auto value) {
//        EXPECT_EQ(sizeof(value), sizeof(int));
//        EXPECT_EQ(value, 23);
//    });
// }

TEST(Variant, which) {
    using V = Variant<char, int, float>;
    V v{};

    ASSERT_EQ(v.which(), type<char>);
    ASSERT_NE(v.which(), type<int>);
    ASSERT_NE(v.which(), type<float>);

    v = 3.2f;

    ASSERT_NE(v.which(), type<char>);
    ASSERT_NE(v.which(), type<int>);
    ASSERT_EQ(v.which(), type<float>);

    static_assert(V::Which::of<char>() == type<char>);
    // Should not compile due to double is not part of V
    // static_assert(V::Which::of<double>() == type<double>);
}

TEST(Variant, visit) {
    auto v = Variant<char, int, float>{};

    auto size0 = v.visit([](auto value) { return sizeof(value); });
    EXPECT_EQ(size0, sizeof(char));

    v = 3.2f;

    auto size1 = v.visit([](auto value) { return sizeof(value); });
    EXPECT_EQ(size1, sizeof(float));
}

TEST(Variant, amendVisit) {
    using V = Variant<char, int, float>;
    V v1(3.14f), v2('c');

    v1.amendVisit([](auto& value) { value += 3.2f; });
    //    v1.amendVisit([](int& value) { value -= 1; }, [](char&) {}, [](float& value) { value += 1; });

    v1.visit([](auto value) {
        EXPECT_EQ(sizeof(value), sizeof(float));
        EXPECT_EQ(value, 3.14f + 3.2f /*+ 1*/);
    });

    v2.amendVisit([](auto& value) { value += 3.2f; });
    //    v2.amendVisit([](int& value) { value -= 1; }, [](char&) {}, [](float& value) { value += 1; });
    v2.visit([](auto value) {
        EXPECT_EQ(sizeof(value), sizeof(char));
        EXPECT_EQ(value, 'f');
    });
}

// TEST(Variant, overloaded) {
//    auto v = Variant<char, int, float>{};

//    auto charVisitor = [](char) { return 1; };
//    auto intVisitor = [](int) { return 2; };
//    auto floatVisitor = [](float) { return 3; };

//    EXPECT_EQ(1, v.visit(charVisitor, intVisitor, floatVisitor));
//    v = 1;
//    EXPECT_EQ(2, v.visit(charVisitor, intVisitor, floatVisitor));
//    v = 3.2f;
//    EXPECT_EQ(3, v.visit(charVisitor, intVisitor, floatVisitor));
//}
