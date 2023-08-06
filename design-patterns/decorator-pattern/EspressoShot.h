#ifndef DECORATOR_PATTERN_ESPRESSOSHOT_H
#define DECORATOR_PATTERN_ESPRESSOSHOT_H


#include "BeverageDecorator.h"

class EspressoShot : public BeverageDecorator {
public:
    EspressoShot(std::unique_ptr<Beverage> bev) : BeverageDecorator(std::move(bev)) {}

    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_ESPRESSOSHOT_H
