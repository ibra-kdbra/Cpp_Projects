#pragma once

namespace meta19 {

// note: MSVC has issues if we pack this into a funtion
// * this function cannot be inlined - resulting in very bad codegen

#ifdef _MSC_VER
#    define META19_UNREACHABLE() __assume(false)
#else
#    define META19_UNREACHABLE() __builtin_unreachable()
#endif

} // namespace meta19
