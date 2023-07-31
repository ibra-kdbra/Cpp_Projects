#ifndef FACTORY_METHOD_PATTERN_ANIMALFACTORY_H
#define FACTORY_METHOD_PATTERN_ANIMALFACTORY_H

#include "Animal.h"

constexpr int NUMBER_OF_ANIMALS = 3;

class AnimalFactory {
public:
    virtual std::shared_ptr<Animal> create() = 0;
};

#endif //FACTORY_METHOD_PATTERN_ANIMALFACTORY_H
