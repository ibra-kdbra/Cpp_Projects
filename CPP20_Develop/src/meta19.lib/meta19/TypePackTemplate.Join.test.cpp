#include "TypePack.h"
#include "TypePackTemplate.Join.h"

#include <type_traits>

using namespace meta19;

void type_pack_join_test() { //
    static_assert(std::is_same_v<
                  JoinTypePacks<TypePack<int, float>, TypePack<char>, TypePack<>, TypePack<double, bool>>,
                  TypePack<int, float, char, double, bool>>);
}
