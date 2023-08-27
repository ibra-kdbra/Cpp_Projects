#ifndef ABSTRACT_FACTORY_PATTERN_PANTS_H
#define ABSTRACT_FACTORY_PATTERN_PANTS_H


#include <string>

class Pants {
public:
    virtual int numOfPockets() = 0;
    virtual std::string description() = 0;
    virtual ~Pants() = default;
};

#endif //ABSTRACT_FACTORY_PATTERN_PANTS_H
