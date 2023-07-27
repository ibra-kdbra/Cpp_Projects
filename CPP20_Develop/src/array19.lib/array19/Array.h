#pragma once
#include "MoveSliceOf.h"
#include "SliceOf.h"

#include <stddef.h> // size_t

namespace array19 {

/// simplified version of std::array
/// * no member types
/// * no exceptions
template<class T, size_t C> struct Array {
    using Element = T;
    static constexpr auto count = C;

    T m[count];

    bool operator==(const Array&) const = default;

    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return false; }
    [[nodiscard]] constexpr auto begin() const noexcept -> const T* { return m; }
    [[nodiscard]] constexpr auto end() const noexcept -> const T* { return m + C; }
    [[nodiscard]] constexpr auto operator[](size_t i) const noexcept -> const T& { return m[i]; }
    [[nodiscard]] constexpr operator SliceOf<const T>() const& noexcept { return SliceOf{m, C}; }
    [[nodiscard]] constexpr auto move() noexcept -> MoveSliceOf<T> { return MoveSliceOf{m, C}; }

    // hint: use `amendSliceOfArray()` if you need to iterate and mutate
    [[nodiscard]] constexpr auto amendBegin() noexcept -> T* { return m; }
    [[nodiscard]] constexpr auto amendEnd() noexcept -> T* { return m + C; }
    [[nodiscard]] constexpr auto amend() noexcept -> SliceOf<T> { return SliceOf{m, C}; }
};

template<class T> struct Array<T, 0u> {
    using Element = T;
    static constexpr auto count = 0u;

    bool operator==(const Array&) const = default;

    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return true; }
    [[nodiscard]] constexpr auto begin() const noexcept -> const T* { return nullptr; }
    [[nodiscard]] constexpr auto end() const noexcept -> const T* { return nullptr; }
    [[nodiscard]] constexpr operator SliceOf<const T>() const& noexcept { return SliceOf{begin(), 0u}; }
    [[nodiscard]] constexpr auto move() noexcept -> MoveSliceOf<T> { return MoveSliceOf{amendBegin(), 0u}; }

    // hint: use `amendSliceOfArray()` if you need to iterate and mutate
    [[nodiscard]] constexpr auto amendBegin() noexcept -> T* { return nullptr; }
    [[nodiscard]] constexpr auto amendEnd() noexcept -> T* { return nullptr; }
    [[nodiscard]] constexpr auto amend() noexcept -> SliceOf<T> { return SliceOf{amendBegin(), 0u}; }
};

/// simplified deduction guide
/// * not checking that all types are the same
/// usage:
///     Array{1, 2, 3};
template<class T, class... Ts> Array(T, Ts...) -> Array<T, 1 + sizeof...(Ts)>;

} // namespace array19
