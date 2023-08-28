#pragma once
#include "SizeArchive.h"
#include "UniqueBuffer.h"
#include "WriteToArchive.h"

namespace serialize19 {

/// serializes T to a dynamically allocated UniqueBuffer
template<EndianBehaviour endian = EndianBehaviour::Keep, class T> auto dynamicWrite(const T& value) -> UniqueBuffer {
    auto sizeArchive = SizeArchive{};
    serialize(sizeArchive, value);
    auto buffer = UniqueBuffer{sizeArchive.size()};
    auto writeArchive = WriteToArchive<endian>{buffer.amendSlice()};
    serialize(writeArchive, value);
    return buffer;
}

} // namespace serialize19
