#ifndef FACTORY_METHOD_PATTERN_CAT_H
#define FACTORY_METHOD_PATTERN_CAT_H

#include "Animal.h"

class Cat : public Animal {
public:
    std::string sound() {
        return "meow";
    }
};

#endif //FACTORY_METHOD_PATTERN_CAT_H
