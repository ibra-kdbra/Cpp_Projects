#ifndef ABSTRACT_FACTORY_PATTERN_TSHIRT_H
#define ABSTRACT_FACTORY_PATTERN_TSHIRT_H


#include "Shirt.h"

class TShirt : public Shirt {
public:
    std::string putOn();
};


#endif //ABSTRACT_FACTORY_PATTERN_TSHIRT_H
