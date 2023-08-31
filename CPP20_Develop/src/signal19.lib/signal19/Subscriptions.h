#pragma once
#include "Subscription.h"

#include <array19/DynamicArrayOf.h>
#include <array19/MoveSliceOf.single.h>
#include <utility>

namespace signal19 {

using array19::DynamicArrayOf;
using array19::moveSliceOfSingle;

/// Holds many subscriptions
///
/// useful if a single entity subscribes to multiple signals.
struct Subscriptions {
    using Array = DynamicArrayOf<Subscription>;

private:
    Array m_array;

public:
    /// \returns true if any of the subscriptions is for the given signal
    auto isSubscribedTo(details::Signal* signal) const {
        for (auto& sub : m_array) {
            if (sub.isForSignal(signal)) {
                return true;
            }
        }
        return false;
    }

    /// stores a new subscription
    ///
    /// note:
    /// * does NOT check for duplicates (subscription is already done)
    auto add(Subscription&& subscription) { m_array.appendMoved(moveSliceOfSingle(std::move(subscription))); }

    /// cancels all the subscriptions
    ///
    /// note:
    /// * does NOT shrink the memory consumption
    auto cancelAll() { m_array.clear(); }
};

} // namespace signal19
