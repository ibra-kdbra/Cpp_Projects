#include "SignalWith.h"

#include <gtest/gtest.h>

using namespace signal19;

TEST(SignalWith, ReferenceLambda) {
    using S = SignalWith<int>;
    auto s = S{};

    s.emit(7);

    auto r = 0u;
    auto h = [&](int v) { r += v; };
    {
        auto c = s.subscribe(h);
        EXPECT_TRUE(c.isForSignal(&s));
        auto s2 = S{};
        EXPECT_FALSE(c.isForSignal(&s2));

        s.emit(13);
        EXPECT_EQ(r, 13u);
    }
    s.emit(17);
    EXPECT_EQ(r, 13u);
}

TEST(SignalWith, RvalueLambda) {
    using S = SignalWith<int>;
    auto s = S{};

    s.emit(7);

    auto r = 0u;
    {
        auto c = s.subscribe([&](int v) { r += v; });

        s.emit(13);
        EXPECT_EQ(r, 13u);
    }
    s.emit(17);
    EXPECT_EQ(r, 13u);
}

TEST(SignalWith, RemoveSignal) {
    using S = SignalWith<int>;
    auto s = new S{};

    s->emit(7);

    auto r = 0u;
    auto c = s->subscribe([&](int v) { r += v; });

    s->emit(13);
    EXPECT_EQ(r, 13u);

    delete s;
}
