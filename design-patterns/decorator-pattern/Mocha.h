#ifndef DECORATOR_PATTERN_MOCHA_H
#define DECORATOR_PATTERN_MOCHA_H


#include "Beverage.h"

class Mocha : public Beverage {
public:
    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_MOCHA_H
