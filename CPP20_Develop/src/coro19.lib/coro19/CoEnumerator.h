#pragma once
#include "coroutine.h"

#include <iterator>
#include <utility>

namespace coro19 {

// use this template as return type to enable co_yield V{}
// to keep things simple promise_type has a copy of V
template<class V> struct CoEnumerator final {
    struct promise_type;
    using Handle = coroutine_handle<promise_type>;

    struct promise_type {
        V v;

        auto get_return_object() noexcept -> CoEnumerator { return CoEnumerator{Handle::from_promise(*this)}; }

        constexpr static auto initial_suspend() noexcept -> suspend_always { return {}; }
        constexpr static auto final_suspend() noexcept -> suspend_always { return {}; }

        auto yield_value(V value) noexcept -> suspend_always {
            v = std::move(value);
            return suspend_always{};
        }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
    };

    auto operator*() const noexcept -> const V& { return handle.promise().v; }
    auto operator->() const noexcept -> const V* { return &handle.promise().v; }
    auto move() -> V { return std::move(handle.promise().v); }

    explicit operator bool() const { return handle && !handle.done(); }
    bool operator++(int) {
        if (handle) handle.resume();
        return static_cast<bool>(*this);
    }
    auto operator++() -> CoEnumerator& {
        if (handle) handle.resume();
        return *this;
    }

    ~CoEnumerator() {
        if (handle) handle.destroy();
    }
    CoEnumerator() = default;
    CoEnumerator(const CoEnumerator&) = delete;
    CoEnumerator& operator=(const CoEnumerator&) = delete;
    CoEnumerator(CoEnumerator&& o) noexcept : handle(o.handle) { o.handle = {}; }
    CoEnumerator& operator=(CoEnumerator&& o) noexcept { return std::swap(handle, o.handle), *this; }

private:
    explicit CoEnumerator(Handle h) : handle(h) {}

private:
    Handle handle{};
};

} // namespace coro19
