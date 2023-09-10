#pragma once
#include "Strong.h"
#include "Strong.ostream.h"

namespace strong19 {

/// using this header will auto define ostream operator for all strong types
/// Please use only for tests!
///
/// To prevent clang-format from auto sorting this header down add a line comment after it
/// Example:
///   #include <strong19/Strong.extras.ostream.h>
///   // keep above include at top!
///   #include "more.h"

#ifdef DEFINE_STRONG_EXTRAS
#    undef DEFINE_STRONG_EXTRAS
#endif

#define DEFINE_STRONG_EXTRAS(NAME) DEFINE_STRONG_OSTREAM(NAME)

} // namespace strong19
