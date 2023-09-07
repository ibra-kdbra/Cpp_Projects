#pragma once
#include "StringView.h"

namespace string19 {

constexpr auto begin(const StringView sv) noexcept -> const char* { return sv.data; }
constexpr auto end(const StringView sv) noexcept -> const char* { return sv.data + sv.count; }

} // namespace string19
