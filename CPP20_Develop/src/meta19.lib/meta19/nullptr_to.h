#pragma once

namespace meta19 {

/// generates a nullptr to given type T
/// usage:
/// - initialize auto variables as pointers
/// - transport type without wrapper (only non reference types!)
template<class T> static constexpr T* nullptr_to [[maybe_unused]] = nullptr;

} // namespace meta19
