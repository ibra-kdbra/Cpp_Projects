#ifndef STRATEGY_PATTERN_FREEFLYINGBEHAVIOR_H
#define STRATEGY_PATTERN_FREEFLYINGBEHAVIOR_H

#include <iostream>
#include "IFlyBehavior.h"

class FreeFlyingBehavior : public IFlyBehavior {
    void fly() {
        std::cout << "I'm freeeeeeee!!! Free flying!!!!" << std::endl;
    }
};

#endif //STRATEGY_PATTERN_FREEFLYINGBEHAVIOR_H
