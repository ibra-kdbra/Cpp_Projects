#ifndef FACTORY_METHOD_PATTERN_RANDOMANIMALFACTORY_H
#define FACTORY_METHOD_PATTERN_RANDOMANIMALFACTORY_H

#include "AnimalFactory.h"

class RandomAnimalFactory : public AnimalFactory {
public:
    RandomAnimalFactory() {
        srand(time(NULL));
    }
    std::shared_ptr<Animal> create();

};


#endif //FACTORY_METHOD_PATTERN_RANDOMANIMALFACTORY_H
