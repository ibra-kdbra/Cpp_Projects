#ifndef FACTORY_METHOD_PATTERN_DUCK_H
#define FACTORY_METHOD_PATTERN_DUCK_H

#include "Animal.h"

class Duck : public Animal {
public:
    std::string sound() {
        return "quack";
    }
};

#endif //FACTORY_METHOD_PATTERN_DUCK_H
