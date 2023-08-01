#pragma once
#include "Enum.h"
#include "Enum.ostream.h"

namespace enum19 {

/// using this header will auto define ostream operator for all enum types
/// Please use only for tests!
///
/// To prevent clang-format from auto sorting this header down add a line comment after it
/// Example:
///   #include <enum19/Enum.extras.ostream.h>
///   // keep above include at top!
///   #include "more.h"

#if defined(ENUM19_EXTRAS)
#    undef ENUM19_EXTRAS
#endif

#define ENUM19_EXTRAS(NAME) ENUM19_NAME_OSTREAM(NAME)

} // namespace enum19
