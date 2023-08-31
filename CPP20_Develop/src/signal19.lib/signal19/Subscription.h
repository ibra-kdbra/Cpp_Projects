#pragma once

namespace signal19 {

struct Subscription;

namespace details {

auto noSubscription() noexcept -> Subscription*;

struct TrackingCancellation;

/// Abstract Base for Cancellations
///
/// note:
/// * always stores a pointer to a Subscription object
struct Cancellation {
    Cancellation() = default;
    virtual ~Cancellation() noexcept = default;
    virtual void cancel() noexcept = 0;
    virtual bool isForSignal(const void* signal) const noexcept = 0;

protected:
    friend struct signal19::Subscription; // subcription updates the pointer, when moved
    Subscription* subscription = noSubscription();
};

/// Implementation of Cancellation that cancels nothing
/// Used as fallback to avoid conditionals
struct NullCancellation final : Cancellation {
    void cancel() noexcept override {}
    bool isForSignal(const void*) const noexcept override { return false; }
};

static auto nullCancellation = NullCancellation{};

/// Base class for Signal to get a bit of type safety
struct Signal {};

} // namespace details

/// Move-Only RAII owner of a subscription
/// used as result type of Signal-subscribe
///
/// note:
/// * completly type erased all details about the signal type
/// * applies cancellation when destroyed
/// * updates the pointer in the cancellation on move
struct Subscription final {
    explicit Subscription(details::Cancellation* cancellation) noexcept : cancellation(cancellation) {
        cancellation->subscription = this;
    }
    Subscription(Subscription&& old) noexcept : cancellation(old.cancellation) {
        old.cancellation = &details::nullCancellation;
        cancellation->subscription = this;
    }
    Subscription& operator=(Subscription&& old) noexcept {
        cancellation->cancel();
        cancellation = old.cancellation;
        cancellation->subscription = this;
        old.cancellation = &details::nullCancellation;
        return *this;
    }
    ~Subscription() noexcept { cancellation->cancel(); }

    /// \returns true if this subscription is for the given signal
    bool isForSignal(const details::Signal* signal) const { return cancellation->isForSignal(signal); }

private:
    friend struct details::TrackingCancellation;
    details::Cancellation* cancellation = &details::nullCancellation;
};

namespace details {

static auto nullSubscription = Subscription{&nullCancellation};

inline auto noSubscription() noexcept -> Subscription* { return &nullSubscription; }

} // namespace details

} // namespace signal19
