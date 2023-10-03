#pragma once
#include <iostream>
#include <vector>
#include "reserved_list.h"

namespace simple_lexser {

class Token_t {
    public:
    int32_t line;
    int32_t index;
    std::string value;
    enum token_type type;

    Token_t(
        int32_t line,
        int32_t index,
        std::string value,
        enum token_type type): line(line), index(index), value(value), type(type) {}
    virtual void to_string() {
        std::cout << "< " << value << " > (L,I)=" << line << "," << index << " " << type << std::endl;
    }
};

}