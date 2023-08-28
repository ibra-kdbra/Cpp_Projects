#pragma once
#include "ArchiveMode.h"

namespace serialize19 {

/// Various kinds of Archives are used for serialisation
/// see:
/// - ReadArchive
/// - SizeArchive
/// - WriteToArchive
template<class A>
concept Archive = requires(A& a, ArchiveMode& mode, int v) {
                      mode = A::mode;
                      a.withPrimitive(v);
                  };

} // namespace serialize19
