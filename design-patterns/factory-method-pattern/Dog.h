#ifndef FACTORY_METHOD_PATTERN_DOG_H
#define FACTORY_METHOD_PATTERN_DOG_H

#include "Animal.h"

class Dog : public Animal {
public:
    std::string sound() {
        return "woof";
    }
};

#endif //FACTORY_METHOD_PATTERN_DOG_H
