#ifndef ABSTRACT_FACTORY_PATTERN_SLACKS_H
#define ABSTRACT_FACTORY_PATTERN_SLACKS_H


#include "Pants.h"

class Slacks : public Pants{
public:
    int numOfPockets();
    std::string description();
};


#endif //ABSTRACT_FACTORY_PATTERN_SLACKS_H
