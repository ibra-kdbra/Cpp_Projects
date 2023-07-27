#pragma once
#include "AllocatedArrayUtils.h"
#include "MoveSliceOf.h"
#include "SliceOf.single.h"

#include <new> // new allocators
#include <stddef.h> // size_t
#include <type_traits> // is_nothrow_*

namespace array19 {

/// allocated array on the heap
/// * count is fixated at construction time
/// * all array elements are initialized (unlike std::vector)
///
/// note:
/// * C++ lacks a new T[] (custom-initializer) thing - so we would not be able to copy without default initializing
///   => to work around this we use ::operator new[] & ::operator delete[] and construct explicitly
template<class T> struct AllocatedArrayOf final {
    using Element = T;
    using Count = size_t;
    using Index = size_t;

    using Iterator = Element*;
    using ConstIterator = const Element*;
    using Slice = SliceOf<Element>;
    using ConstSlice = SliceOf<const Element>;
    using MoveSlice = MoveSliceOf<Element>;

private:
    using Utils = AllocatedArrayUtils<T>;
    T* m_pointer{};
    size_t m_count{};

public:
    AllocatedArrayOf() = default;
    ~AllocatedArrayOf() noexcept {
        if (m_pointer) {
            Utils::destruct(amend());
            Utils::deallocate(amend());
        }
    }

    explicit AllocatedArrayOf(ConstSlice slice) : m_pointer(Utils::allocate(slice.count())), m_count(0) {
        Utils::copyConstruct(m_pointer, slice);
        m_count = slice.count();
    }

    explicit AllocatedArrayOf(MoveSlice slice) : m_pointer(Utils::allocate(slice.count())), m_count(0) {
        Utils::moveConstruct(m_pointer, slice);
        m_count = slice.count();
    }

    template<class... Ts> requires(sizeof...(Ts) > 0) && requires(Ts&&... args) { (T{(Ts &&) args}, ...); }
    explicit AllocatedArrayOf(Ts&&... args) : m_pointer(Utils::allocate(sizeof...(Ts)))
                                            , m_count(0) {
        (new (m_pointer + m_count++) T{(Ts &&) args}, ...);
    }

    AllocatedArrayOf(const AllocatedArrayOf& o) : AllocatedArrayOf(static_cast<ConstSlice>(o)) {}

    AllocatedArrayOf& operator=(const AllocatedArrayOf& o) {
        if (!m_pointer) {
            *this = AllocatedArrayOf(o);
        }
        else if (o.m_count != m_count) {
            Utils::destruct(amend());
            Utils::deallocate(amend());
            if (0 == o.m_count) {
                m_pointer = nullptr;
                m_count = 0;
            }
            else {
                *this = AllocatedArrayOf(o);
            }
        }
        else if (0 < m_count) {
            Utils::copyAssign(m_pointer, o);
        }
        return *this;
    }

    AllocatedArrayOf(AllocatedArrayOf&& o) noexcept //
            : m_pointer(o.m_pointer)
            , m_count(o.m_count) {
        o.m_pointer = nullptr;
    }
    AllocatedArrayOf& operator=(AllocatedArrayOf&& o) noexcept {
        if (m_pointer) {
            Utils::destruct(amend());
            Utils::deallocate(amend());
        }
        m_pointer = o.m_pointer;
        m_count = o.m_count;
        o.m_pointer = nullptr;
        return *this;
    }

    [[nodiscard]] static auto createCount(Count count) -> AllocatedArrayOf {

        auto result = AllocatedArrayOf{};
        if (count > 0) {
            result.m_pointer = Utils::allocate(count);
            Utils::defaultConstruct(Slice{result.m_pointer, count});
            result.m_count = count;
        }
        return result;
    }

    [[nodiscard]] auto isEmpty() const noexcept -> bool { return m_count == 0; }
    [[nodiscard]] auto count() const noexcept -> Count { return m_count; }

    [[nodiscard]] auto front() const -> const T& { return *begin(); }
    [[nodiscard]] auto back() const -> const T& { return *(begin() + m_count - 1); }

    [[nodiscard]] auto begin() const noexcept -> ConstIterator { return std::launder(m_pointer); }
    [[nodiscard]] auto end() const noexcept -> ConstIterator { return begin() + m_count; }
    [[nodiscard]] auto operator[](Index index) const noexcept -> const T& { return *std::launder(m_pointer + index); }
    operator ConstSlice() const noexcept { return SliceOf{begin(), m_count}; }

    // hint: use `amend()` if you need to iterate and mutate
    [[nodiscard]] auto amendBegin() noexcept -> Iterator { return std::launder(m_pointer); }
    [[nodiscard]] auto amendEnd() noexcept -> Iterator { return amendBegin() + m_count; }
    [[nodiscard]] auto amend() noexcept -> Slice { return SliceOf{amendBegin(), m_count}; }
};

/// simplified deduction guide
/// usage:
///     AllocatedArrayOf{1,2,3}
template<class T, class... Ts> AllocatedArrayOf(T&&, Ts&&...) -> AllocatedArrayOf<T>;

/// deduce SliceOf from AllocatedArrayOf
/// usage:
///     auto a = AllocatedArrayOf{1,2,3};
///     auto slice = SliceOf{a};
template<class T> SliceOf(const AllocatedArrayOf<T>&) -> SliceOf<const T>;

} // namespace array19
