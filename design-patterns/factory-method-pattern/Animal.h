#ifndef FACTORY_METHOD_PATTERN_ANIMAL_H
#define FACTORY_METHOD_PATTERN_ANIMAL_H

#include <string>

class Animal {
public:
    virtual std::string sound() = 0;
};

#endif //FACTORY_METHOD_PATTERN_ANIMAL_H
