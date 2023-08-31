#include "Subscriptions.h"

#include "SignalWith.h"

#include <gtest/gtest.h>

using namespace signal19;

TEST(Subscriptions, Example) {
    auto subs = Subscriptions{};

    auto s1 = SignalWith<int>{};
    auto r = 0;
    subs.add(s1.subscribe([&r](int x) { r += x; }));

    auto s2 = SignalWith<float>{};

    EXPECT_FALSE(subs.isSubscribedTo(&s2));

    subs.add(s2.subscribe([](float) {}));

    EXPECT_TRUE(subs.isSubscribedTo(&s1));
    EXPECT_TRUE(subs.isSubscribedTo(&s2));

    subs.cancelAll();
    EXPECT_FALSE(subs.isSubscribedTo(&s1));
    EXPECT_FALSE(subs.isSubscribedTo(&s2));
}
