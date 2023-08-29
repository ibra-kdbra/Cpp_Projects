#pragma once
#include "Endianess.h"
#include "array19/SliceOf.h"

#include <stdint.h> // uint8_t

namespace serialize19 {

using array19::SliceOf;

namespace details {

struct WriteAppenderCommon {
    constexpr WriteAppenderCommon() = default;
    constexpr explicit WriteAppenderCommon(uint8_t* data) : m_data(data) {}

protected:
    uint8_t* m_data{};
};
} // namespace details

/// appends primitive values to a given pointer
/// note: caller has to ensure that enough storage is available (see dynamicWrite)
template<EndianBehaviour> struct WriteAppender;

template<> struct WriteAppender<EndianBehaviour::Keep> final : details::WriteAppenderCommon {
    using details::WriteAppenderCommon::WriteAppenderCommon;

    template<class T> auto appendValue(T value) const -> WriteAppender {
        *(T*)m_data = value;
        return WriteAppender{m_data + sizeof(T)};
    }
    template<class T> auto appendSlice(SliceOf<const T> slice) const -> WriteAppender {
        auto p = (T*)m_data;
        for (auto v : slice) *p++ = v;
        return WriteAppender{(uint8_t*)p};
    }
};

template<> struct WriteAppender<EndianBehaviour::Flip> final : details::WriteAppenderCommon {
    using details::WriteAppenderCommon::WriteAppenderCommon;

    template<class T> auto appendValue(T value) const -> WriteAppender {
        *(T*)m_data = endianFlipFor<T>(value);
        return WriteAppender{m_data + sizeof(T)};
    }
    template<class T> auto appendSlice(SliceOf<const T> slice) const -> WriteAppender {
        auto p = (T*)m_data;
        for (auto v : slice) *p++ = endianFlipFor<T>(v);
        return WriteAppender{(uint8_t*)p};
    }
};

} // namespace serialize19
