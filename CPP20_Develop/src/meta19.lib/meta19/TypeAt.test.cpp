#include "TypeAt.h"

using namespace meta19;

void test_TypeAt() {
    static_assert(std::is_same_v<int, TypeAt<1, char, int, float>>);
    static_assert(std::is_same_v<Index<1>, IndexOf<int, char, int, float>>);
    static_assert(1 == index_of<int, char, int, float>);
}

void test_TypeAtMap() {
    using Map = IndexTypeMap<char, int, float>;
    static_assert(std::is_same_v<int, TypeAtMap<1, Map>>);
    static_assert(std::is_same_v<Index<1>, IndexOfMap<int, Map>>);
    static_assert(1 == index_of_map<int, Map>);
}
