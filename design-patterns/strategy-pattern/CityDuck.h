#ifndef STRATEGY_PATTERN_CITYDUCK_H
#define STRATEGY_PATTERN_CITYDUCK_H


#include "Duck.h"
#include "NoQuackBehavior.h"
#include "FancyFlyingBehavior.h"

class CityDuck : public Duck {
public:
    CityDuck() : Duck(std::make_shared<NoQuackBehavior>(), std::make_shared<FancyFlyingBehavior>()) {
    }
};


#endif //STRATEGY_PATTERN_CITYDUCK_H
