// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef SYNC_HPP_
#define SYNC_HPP_

#include <mutex>
#include <chrono>
#include <thread>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <list>
#include <utility>

namespace tycho {
using sync_timepoint = std::chrono::steady_clock::time_point;

inline auto sync_clock(unsigned timeout = 0) {
    return std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);
}

inline auto sync_sleep(sync_timepoint timepoint) {
    std::this_thread::sleep_until(timepoint);
}

inline auto sync_yield() {
    std::this_thread::yield();
}

inline auto sync_duration(const sync_timepoint& start, const sync_timepoint& end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

inline auto sync_elapsed(const sync_timepoint& start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(sync_clock() - start);
}

inline auto sync_remains(const sync_timepoint& end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - sync_clock());
}

template <typename T>
class unique_sync final {
public:
    template <typename... Args>
    explicit unique_sync(Args&&... args) :
    data(std::forward<Args>(args)...) {}

private:
    template <typename U> friend class sync_ptr;
    T data{};
    std::mutex lock;
};

template <typename T>
class shared_sync final {
public:
    template <typename... Args>
    explicit shared_sync(Args&&... args) :
    data(std::forward<Args>(args)...) {}

private:
    template <typename U> friend class reader_ptr;
    template <typename U> friend class writer_ptr;
    T data{};
    std::shared_mutex lock;
};

template <typename U>
class sync_ptr final {
public:
    sync_ptr() = delete;
    sync_ptr(const sync_ptr&) = delete;
    auto operator=(const sync_ptr&) = delete;

    explicit sync_ptr(unique_sync<U>& obj) : sync_(obj), ptr_(&obj.data) {
        sync_.lock.lock();
    }

    ~sync_ptr() {
        sync_.lock.unlock();
    }

    inline auto operator->() {
        return ptr_;
    }

    inline auto operator*() -> U& {
        return *ptr_;
    }

private:
    unique_sync<U> &sync_;
    U* ptr_;
};

template <typename U>
class reader_ptr final {
public:
    reader_ptr() = delete;
    reader_ptr(const reader_ptr&) = delete;
    auto operator=(const reader_ptr&) = delete;

    explicit reader_ptr(shared_sync<U>& obj) : sync_(obj), ptr_(&obj.data) {
        sync_.lock.lock_shared();
    }

    ~reader_ptr() {
        sync_.lock.unlock_shared();
    }

    inline auto operator->() const {
        return ptr_;
    }

    inline auto operator*() const -> const U& {
        return *ptr_;
    }

private:
    shared_sync<U> &sync_;
    const U* ptr_;
};

template <typename U>
class writer_ptr final {
public:
    writer_ptr() = delete;
    writer_ptr(const writer_ptr&) = delete;
    auto operator=(const writer_ptr&) = delete;

    explicit writer_ptr(shared_sync<U>& obj) : sync_(obj), ptr_(&obj.data) {
        sync_.lock.lock();
    }

    ~writer_ptr() {
        sync_.lock.unlock();
    }

    inline auto operator->() {
        return ptr_;
    }

    inline auto operator*() -> U& {
        return *ptr_;
    }

private:
    shared_sync<U> &sync_;
    U* ptr_;
};

class defer final {
public:
    defer() = delete;
    defer(const defer&) = delete;
    auto operator=(const defer&) = delete;

    explicit defer(std::function<void()> func) : action_(std::move(func)) {}
    ~defer() {action_();}

private:
    std::function<void()> action_;
};

class semaphore_t final {
public:
    semaphore_t(const semaphore_t&) = delete;
    auto operator=(const semaphore_t&) = delete;

    inline explicit semaphore_t(unsigned limit = 1) : limit_(limit) {}

    inline ~semaphore_t() {
        release();
        std::this_thread::yield();
        while(count_) {
            std::this_thread::yield();
        }
    }

    inline void release() {
        const std::unique_lock lock(lock_);
        release_ = true;
        if(count_ > limit_)
            cond_.notify_all();
    }

    inline void post() {
        const std::unique_lock lock(lock_);
        if(--count_ >= limit_)
            cond_.notify_one();
    }

    inline void wait() {
        std::unique_lock lock(lock_);
        while(!release_ && count_ > limit_)
            cond_.wait(lock);
    }

    inline auto wait_until(sync_timepoint timepoint) {
        std::unique_lock lock(lock_);
        while(!release_ && count_ > limit_) {
            if(cond_.wait_until(lock, timepoint) == std::cv_status::timeout)
                return false;
        }
        return true;
    }

    inline auto pending() const {
        const std::unique_lock lock(lock_);
        if(count_ > limit_)
            return limit_ - count_;
        return 0U;
    }

    inline auto count() const {
        const std::unique_lock lock(lock_);
        return count_;
    }

    inline auto active() const {
        const std::unique_lock lock(lock_);
        if(count_ <= limit_)
            return count_;
        return limit_;
    }

private:
    mutable std::mutex lock_;
    std::condition_variable cond_;
    unsigned count_{0}, limit_;
    bool release_{false};
};

class barrier_t final {
public:
    barrier_t(const barrier_t&) = delete;
    auto operator=(const barrier_t&) = delete;

    inline explicit barrier_t(unsigned limit = 1) : limit_(limit) {}

    inline ~barrier_t() {
        release();
    }

    inline void release() {
        const std::unique_lock lock(lock_);
        if(count_)
            cond_.notify_all();
    }

    inline void wait() {
        std::unique_lock lock(lock_);
        if(++count_ >= limit_) {
            cond_.notify_all();
            return;
        }
        cond_.wait(lock);
        if(++ending_ < limit_)
            return;
        count_ = ending_ = 0;
    }

    inline auto count() const {
        const std::unique_lock lock(lock_);
        return count_;
    }

    inline auto pending() const {
        const std::unique_lock lock(lock_);
        return count_ - ending_;
    }

private:
    mutable std::mutex lock_;
    std::condition_variable cond_;
    unsigned count_{0}, ending_{0}, limit_;
};

template <typename T>
class sync_action final {
public:
    using actor_t = void (*)(const T& body);
    using msg_t = std::pair<actor_t, T>;

    sync_action(const sync_action&) = delete;
    auto operator=(const sync_action&) = delete;
    inline sync_action() = default;

    inline ~sync_action() {
        cvar_.notify_all();
    }

    inline void send(const T& body, actor_t actor) {
        std::unique_lock<std::mutex> lock(lock_);
        msgs_.emplace_back(actor, body);
        lock.unlock();
        cvar_.notify_one();
    }

    inline void wait() {
        std::unique_lock<std::mutex> lock(lock_);
        while(msgs_.empty())
            cvar_.wait(lock);

        auto copy = std::move(msgs_.front());
        auto [actor, body] = copy;
        msgs_.pop_front();
        lock.unlock();
        actor(body);
    }

    inline auto wait_until(sync_timepoint timepoint) {
        std::unique_lock<std::mutex> lock(lock_);
        while(msgs_.empty()) {
            if(cvar_.wait_until(lock, timepoint) == std::cv_status::timeout)
                return false;
        }

        auto copy = std::move(msgs_.front());
        auto [actor, body] = copy;
        msgs_.pop_front();
        lock.unlock();
        actor(body);
        return true;
    }

private:
    mutable std::mutex lock_;
    unsigned signal_{0};
    std::condition_variable cvar_;
    std::list<msg_t> msgs_;
};
} // end namespace

/*!
 * Thread sync and actors framework.  This offers a simple and constent set
 * of synchronization primitives and a simple actor framework.
 * \file sync.hpp
 */
#endif
