#ifndef STRATEGY_PATTERN_COUNTRYDUCK_H
#define STRATEGY_PATTERN_COUNTRYDUCK_H

#include "Duck.h"
#include "SimpleQuackBehavior.h"
#include "FreeFlyingBehavior.h"

class CountryDuck : public Duck {
public:
    CountryDuck() : Duck(std::make_shared<SimpleQuackBehavior>(), std::make_shared<FreeFlyingBehavior>()) {

    }
};

#endif //STRATEGY_PATTERN_COUNTRYDUCK_H
