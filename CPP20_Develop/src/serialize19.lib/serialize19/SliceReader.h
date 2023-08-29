#pragma once
#include "BufferSlice.h"
#include "Endianess.h"
#include "meta19/Type.h"

#include <stddef.h> // size_t

namespace serialize19 {

using meta19::Type;

namespace details {

struct SliceReaderCommon {
    constexpr SliceReaderCommon() = default;
    constexpr explicit SliceReaderCommon(const uint8_t* data, size_t count) : m_data(data), m_count(count) {}
    constexpr SliceReaderCommon(BufferSlice slice) : SliceReaderCommon(slice.begin(), slice.count()) {}

    constexpr auto count() const -> size_t { return m_count; }
    constexpr operator BufferSlice() const { return BufferSlice{m_data, m_count}; }

    auto sliceBytes(size_t count) const -> BufferSlice { return SliceOf{m_data, count}; }
    constexpr auto limitBytes(size_t count) const -> SliceReaderCommon { return SliceReaderCommon{m_data, count}; }

    constexpr auto skipBytes(size_t count) const -> SliceReaderCommon {
        return SliceReaderCommon{m_data + count, m_count - count};
    }
    template<class T> constexpr auto skipOf(Type<T>* = {}) const -> SliceReaderCommon {
        return SliceReaderCommon{m_data + sizeof(T), m_count - sizeof(T)};
    }
    template<class T> constexpr auto skipCountOf(size_t count, Type<T>* = {}) const -> SliceReaderCommon {
        return SliceReaderCommon{m_data + count * sizeof(T), m_count - count * sizeof(T)};
    }

protected:
    const uint8_t* m_data{};
    size_t m_count{};
};

} // namespace details

template<EndianBehaviour = EndianBehaviour::Keep> struct SliceReader;

template<> struct SliceReader<EndianBehaviour::Keep> final : details::SliceReaderCommon {
    using details::SliceReaderCommon::SliceReaderCommon;
    using details::SliceReaderCommon::operator=;

    template<class T> constexpr auto as(Type<T>* = {}) const -> T { return *(const T*)m_data; }
    template<class T> auto sliceCountOf(size_t count, Type<T>* = {}) const -> SliceOf<const T> {
        return {(const T*)m_data, count};
    }
    template<class T, class F> void eachCountOf(size_t count, Type<T>*, F&& f) const {
        auto p = (const T*)m_data;
        for (auto i = 0u; i < count; ++i) f(*p++);
    }
};

template<> struct SliceReader<EndianBehaviour::Flip> final : details::SliceReaderCommon {
    using details::SliceReaderCommon::SliceReaderCommon;
    using details::SliceReaderCommon::operator=;

    template<class T> constexpr auto as(Type<T>* = {}) const -> T { return endianFlipFor<T>(*(const T*)m_data); }

    template<class T, class F> void eachCountOf(size_t count, Type<T>*, F&& f) const {
        auto p = (const T*)m_data;
        for (auto i = 0u; i < count; ++i) f(endianFlipFor<T>(*p++));
    }
};

} // namespace serialize19
