#ifndef DECORATOR_PATTERN_CARAMEL_H
#define DECORATOR_PATTERN_CARAMEL_H


#include "BeverageDecorator.h"

class Caramel : public BeverageDecorator {
public:
    Caramel(std::unique_ptr<Beverage> bev) : BeverageDecorator(std::move(bev)) {}

    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_CARAMEL_H
