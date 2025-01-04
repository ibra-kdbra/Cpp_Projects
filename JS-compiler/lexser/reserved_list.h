#pragma once
#include <iostream>

namespace simple_lexser {
    extern const char *const reserved_list;
    extern std::string find_reserve_by_index(int32_t index);
    extern int32_t find_index_by_reserve(std::string str);
    enum token_type {
        RESERVE = 0,
        NUM,
        STRING,
        IDENTIFIER,
        END,
    };
}
