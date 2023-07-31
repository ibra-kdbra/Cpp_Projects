#ifndef FACTORY_METHOD_PATTERN_BALANCEDANIMALFACTORY_H
#define FACTORY_METHOD_PATTERN_BALANCEDANIMALFACTORY_H

#include "AnimalFactory.h"

class BalancedAnimalFactory : public AnimalFactory {
public:
    BalancedAnimalFactory(): next(-1) {}

    std::shared_ptr<Animal> create();

private:
    int next;
};


#endif //FACTORY_METHOD_PATTERN_BALANCEDANIMALFACTORY_H
