#ifndef ABSTRACT_FACTORY_PATTERN_CARGOPANTS_H
#define ABSTRACT_FACTORY_PATTERN_CARGOPANTS_H

#include "Pants.h"

class CargoPants : public Pants {
public:
    int numOfPockets();
    std::string description();
};


#endif //ABSTRACT_FACTORY_PATTERN_CARGOPANTS_H
