#ifndef STRATEGY_PATTERN_NOQUACKBEHAVIOR_H
#define STRATEGY_PATTERN_NOQUACKBEHAVIOR_H


#include "IQuackBehavior.h"

class NoQuackBehavior : public IQuackBehavior {
    void quack(){}
};


#endif //STRATEGY_PATTERN_NOQUACKBEHAVIOR_H
