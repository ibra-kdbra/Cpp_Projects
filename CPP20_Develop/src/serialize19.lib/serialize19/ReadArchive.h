#pragma once
#include "Archive.h"
#include "SliceReader.h"
#include "meta19/Type.h"
#include "serialize.h"

#include <stddef.h> // size_t

namespace serialize19 {

using meta19::type;

/// Archive Adapter that allows reading from serialize() methods
/// see SliceReader
template<EndianBehaviour endian = EndianBehaviour::Keep> struct ReadArchive {
    static constexpr auto mode = ArchiveMode::Read;

    ReadArchive(BufferSlice slice) : m_slice(slice) {}

    auto slice() const -> BufferSlice { return m_slice; }

    auto withSlice(size_t n) -> BufferSlice {
        auto r = m_slice.sliceBytes(n);
        m_slice = m_slice.skipBytes(n);
        return r;
    }

    template<class T> void withPrimitive(T& value) {
        value = m_slice.as(type<T>);
        m_slice = m_slice.skipOf(type<T>);
    }

    void withPrimitive(bool& value) {
        value = m_slice.as(type<uint8_t>) == 1;
        m_slice = m_slice.skipOf(type<uint8_t>);
    }

private:
    SliceReader<endian> m_slice;
};

static_assert(Archive<ReadArchive<>>);

} // namespace serialize19
