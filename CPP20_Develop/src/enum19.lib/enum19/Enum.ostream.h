#pragma once
#include "Enum.names.h"

#include <ostream>
#include <string19/StringView.ostream.h>

namespace enum19 {

template<class Enum> auto enumNameOstream(std::ostream& out, const Enum& e) -> std::ostream& {
    using Underlying = std::underlying_type_t<Enum>;
    auto underlying = static_cast<Underlying>(e);
    using Numeric = std::conditional_t<std::is_signed_v<Underlying>, int64_t, uint64_t>;
    return out << valueName(e) << " (" << static_cast<Numeric>(underlying) << ")";
}

/// This wrapper enables ADL for ostream operator
template<class T> requires(std::is_enum_v<T>) struct EnumName {
    T v;
};
template<class T> EnumName(T) -> EnumName<T>;

template<class T> auto operator<<(std::ostream& out, EnumName<T> wrapper) -> std::ostream& {
    return enumNameOstream(out, wrapper.v);
}

/// defines an ostream operator for given enum type
#define ENUM19_NAME_OSTREAM(NAME)                                                                                      \
    inline auto operator<<(std::ostream& out, const NAME& s)->std::ostream& { return enum19::enumNameOstream(out, s); }

} // namespace enum19
