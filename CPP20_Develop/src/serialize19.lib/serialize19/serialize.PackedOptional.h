#pragma once
#include "optional19/PackedOptional.h"
#include "serialize.h"

namespace serialize19 {

template<Archive A, auto m, auto i> void serialize(A& a, optional19::PackedOptional<m, i>& packedOptional) {
    serialize(a, packedOptional.amend()); // always store the value only as this also encodes the optionality
}

} // namespace serialize19
