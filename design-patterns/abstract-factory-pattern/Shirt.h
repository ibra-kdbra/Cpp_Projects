#ifndef ABSTRACT_FACTORY_PATTERN_SHIRT_H
#define ABSTRACT_FACTORY_PATTERN_SHIRT_H

#include <string>

class Shirt {
public:
    virtual std::string putOn() = 0;

    virtual ~Shirt() = default;
};

#endif //ABSTRACT_FACTORY_PATTERN_SHIRT_H
