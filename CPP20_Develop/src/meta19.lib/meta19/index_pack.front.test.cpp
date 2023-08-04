#include "IndexPack.h"
#include "index_pack.front.h"

using namespace meta19;

void pack_front_test() { //
    static_assert(pack_front<12, 13> == 12);
}

void index_pack_front_test() {
    static_assert(index_pack_front<IndexPack<12, 13>> == 12);
    static_assert(index_pack_front<IndexPack<>> == nullptr);
}
