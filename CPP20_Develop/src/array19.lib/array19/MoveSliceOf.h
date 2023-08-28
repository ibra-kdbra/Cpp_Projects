#pragma once
#include "SliceOf.h"

#include <stddef.h> // size_t

namespace array19 {

/// Same as SliceOf but this marks that elements want to be moved from
/// useful for initializers
template<class T> struct MoveSliceOf {
    using Element = T;
    using Count = size_t;
    using Index = size_t;

private:
    Element* m_data{};
    Count m_count{};

public:
    constexpr MoveSliceOf() = default;
    constexpr explicit MoveSliceOf(Element* data, Count count) noexcept : m_data(data), m_count(count) {}

    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return m_count == 0; }
    [[nodiscard]] constexpr auto count() const noexcept -> Count { return m_count; }
    [[nodiscard]] constexpr auto slice(Index offset, Count count) const noexcept -> MoveSliceOf {
        return MoveSliceOf{m_data + offset, count};
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> Element* { return m_data; }
    [[nodiscard]] constexpr auto end() const noexcept -> Element* { return m_data + m_count; }
    [[nodiscard]] constexpr auto operator[](Index index) const noexcept -> Element&& { return *(m_data + index); }

    [[nodiscard]] constexpr operator SliceOf<const T>() const noexcept { return SliceOf<const T>{m_data, m_count}; }
};

template<class T> MoveSliceOf(T*, size_t) -> MoveSliceOf<T>;

} // namespace array19
