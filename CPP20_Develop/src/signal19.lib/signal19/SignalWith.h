#pragma once
#include "Subscription.h"
#include "array19/DynamicArrayOf.h"

#include <algorithm>
#include <utility>

namespace signal19 {

using array19::DynamicArrayOf;

namespace details {

/// Partial implementation of cancellation
/// implements the updates to the cancellation pointers in the subscription
/// note:
/// * extracted to avoid unnecessary template instantiations of this
struct TrackingCancellation : Cancellation {
    TrackingCancellation() = default;
    TrackingCancellation(TrackingCancellation&& old) noexcept {
        subscription = old.subscription;
        old.subscription = &nullSubscription;
        subscription->cancellation = this;
    }
    TrackingCancellation& operator=(TrackingCancellation&& old) noexcept {
        subscription = old.subscription;
        old.subscription = &nullSubscription;
        subscription->cancellation = this;
        return *this;
    }
    virtual ~TrackingCancellation() noexcept { subscription->cancellation = &nullCancellation; }
};

} // namespace details

/// Implemenation of a signal with given arguments
/// note:
/// * move only because uniquly owns the cancellations
template<class... Args> struct SignalWith final : details::Signal {
private:
    struct StoredCancellation;
    using Vec = DynamicArrayOf<StoredCancellation>;

    /// partial implementation that implements the cancellation
    /// This also is the interface for the internal type erasure.
    struct SignalCancellation : details::TrackingCancellation {
        SignalWith& forSignal;

        explicit SignalCancellation(SignalWith& signal) noexcept : forSignal(signal) {}

        // Cancellation interface
        void cancel() noexcept final override {
            auto it = std::find_if(forSignal.vec.amendBegin(), forSignal.vec.amendEnd(), [this](auto& ptr) {
                return &ptr.m() == this;
            });
            forSignal.vec.remove(it, 1);
        }
        bool isForSignal(const void* signal) const noexcept final override { return &forSignal == signal; }

        virtual void emit(Args... args) const = 0;
        virtual void moveTo(void*) noexcept = 0;
    };

    /// final implementation of the cancellation
    /// This stores the callback and implements the type erasure interface
    template<class Cb> struct CallbackCancellation final : SignalCancellation {
        Cb cb;

        explicit CallbackCancellation(SignalWith& signal, Cb cb) noexcept
                : SignalCancellation(signal)
                , cb(std::move(cb)) {}

        void emit(Args... args) const override { cb(args...); }

        void moveTo(void* ptr) noexcept override { new (ptr) CallbackCancellation(std::move(*this)); }
    };
    template<class Cb> struct CallbackCancellation<Cb&&> final : SignalCancellation {
        Cb cb;

        explicit CallbackCancellation(SignalWith& signal, Cb cb) noexcept
                : SignalCancellation(signal)
                , cb(std::move(cb)) {}

        void emit(Args... args) const override { cb(args...); }

        void moveTo(void* ptr) noexcept override { new (ptr) CallbackCancellation(std::move(*this)); }
    };

    /// Type Erasure for CallbackCancellation
    struct StoredCancellation {
        using Storage =
            std::aligned_storage_t<sizeof(SignalCancellation) + 2 * sizeof(void*), alignof(SignalCancellation)>;
        Storage storage; // fixed size storage

        template<class Cb> explicit StoredCancellation(SignalWith& signal, Cb&& cb) {
            using Clean = std::remove_const_t<std::remove_reference_t<Cb>>;
            static_assert(sizeof(CallbackCancellation<Clean>) <= sizeof(storage));
            new (&storage) CallbackCancellation<Clean>(signal, std::forward<Cb>(cb));
        }

        StoredCancellation(StoredCancellation&& old) noexcept { old.update()->moveTo(&storage); }
        StoredCancellation& operator=(StoredCancellation&& old) noexcept {
            update()->~SignalCancellation();
            old.update()->moveTo(&storage);
            return *this;
        }
        StoredCancellation(const StoredCancellation&) = delete;
        StoredCancellation& operator=(const StoredCancellation&) = delete;

        ~StoredCancellation() noexcept { update()->~SignalCancellation(); }

        // access storage as interface
        auto m() const -> const SignalCancellation& {
            return *std::launder(reinterpret_cast<const SignalCancellation*>(&storage));
        }
        auto update() -> SignalCancellation* { return std::launder(reinterpret_cast<SignalCancellation*>(&storage)); }
    };

    Vec vec;

public:
    SignalWith() = default;
    SignalWith(const SignalWith&) = delete;
    SignalWith& operator=(const SignalWith&) = delete;
    SignalWith(SignalWith&&) = default;
    SignalWith& operator=(SignalWith&&) = default;

    /// invoke every valid subscription
    void emit(Args... args) const {
        for (auto& stored : vec) {
            stored.m().emit(args...);
        }
    }

    /// add given callback as a new subcription
    template<class Cb> //
    [[nodiscard]] auto subscribe(Cb&& cb) -> Subscription {
        auto& stored = vec.emplace_back(*this, std::forward<Cb>(cb));
        return Subscription{stored.update()};
    }
};

} // namespace signal19
