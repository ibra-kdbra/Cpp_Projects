#ifndef DECORATOR_PATTERN_BEVERAGEDECORATOR_H
#define DECORATOR_PATTERN_BEVERAGEDECORATOR_H

#include "Beverage.h"

class BeverageDecorator : public Beverage {
public:
    BeverageDecorator(std::unique_ptr<Beverage> bev): beverage(std::move(bev)) {}

protected:
    std::unique_ptr<Beverage> beverage;
};

#endif //DECORATOR_PATTERN_BEVERAGEDECORATOR_H
