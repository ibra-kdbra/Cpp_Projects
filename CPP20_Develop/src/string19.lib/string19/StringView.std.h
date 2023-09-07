#pragma once
#include "StringView.h"

#include <string_view>

namespace string19 {

constexpr auto viewStd(const std::string_view& sv) -> StringView {
    return {sv.data(), static_cast<unsigned>(sv.size()), ExtraZero::Missing};
}

constexpr auto toStd(const StringView& sv) -> std::string_view { return std::string_view{sv.data, sv.count}; }

} // namespace string19
