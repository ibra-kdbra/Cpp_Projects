#pragma once
#include "StringView.h"

#include <ostream>

namespace string19 {

inline auto operator<<(std::ostream& out, const StringView v) -> std::ostream& { return out.write(v.data, v.count); }

} // namespace string19
