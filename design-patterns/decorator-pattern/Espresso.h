#ifndef DECORATOR_PATTERN_ESPRESSO_H
#define DECORATOR_PATTERN_ESPRESSO_H


#include "Beverage.h"

class Espresso : public Beverage {
public:
    std::string description();
    double cost();
};


#endif //DECORATOR_PATTERN_ESPRESSO_H
