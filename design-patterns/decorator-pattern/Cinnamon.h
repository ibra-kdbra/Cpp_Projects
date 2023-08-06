#ifndef DECORATOR_PATTERN_CINNAMON_H
#define DECORATOR_PATTERN_CINNAMON_H


#include "BeverageDecorator.h"

class Cinnamon : public BeverageDecorator {
public:
    Cinnamon(std::unique_ptr<Beverage> bev) : BeverageDecorator(std::move(bev)) {}

    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_CINNAMON_H
