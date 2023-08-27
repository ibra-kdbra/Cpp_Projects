#ifndef ABSTRACT_FACTORY_PATTERN_CLOTHESFACTORY_H
#define ABSTRACT_FACTORY_PATTERN_CLOTHESFACTORY_H

#include "Shoe.h"
#include "Shirt.h"
#include "Pants.h"

class ClothesFactory {
public:
    virtual std::unique_ptr<Shoe> shoe() = 0;
    virtual std::unique_ptr<Shirt> shirt() = 0;
    virtual std::unique_ptr<Pants> pants() = 0;
};

#endif //ABSTRACT_FACTORY_PATTERN_CLOTHESFACTORY_H
