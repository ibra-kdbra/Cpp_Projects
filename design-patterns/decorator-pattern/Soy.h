#ifndef DECORATOR_PATTERN_SOY_H
#define DECORATOR_PATTERN_SOY_H


#include "BeverageDecorator.h"

class Soy : public BeverageDecorator {
public:
    Soy(std::unique_ptr<Beverage> bev) : BeverageDecorator(std::move(bev)) {}

    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_SOY_H
