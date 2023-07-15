#ifndef STRATEGY_PATTERN_DUCK_H
#define STRATEGY_PATTERN_DUCK_H

#include <memory>
#include "IQuackBehavior.h"
#include "IFlyBehavior.h"

class Duck {
public:

    Duck(std::shared_ptr<IQuackBehavior> _quacker, std::shared_ptr<IFlyBehavior> _flyer):
        quacker(_quacker), flyer(_flyer) {}

    void quack() {
        this->quacker->quack();
    }

    void fly() {
        this->flyer->fly();
    }

private:
    std::shared_ptr<IQuackBehavior> quacker;
    std::shared_ptr<IFlyBehavior> flyer;

};


#endif //STRATEGY_PATTERN_DUCK_H
