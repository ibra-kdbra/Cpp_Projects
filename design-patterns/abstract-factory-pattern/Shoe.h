#ifndef ABSTRACT_FACTORY_PATTERN_SHOE_H
#define ABSTRACT_FACTORY_PATTERN_SHOE_H

#include <string>

class Shoe {
public:
    virtual std::string clean() = 0;

    virtual ~Shoe() = default;
};

#endif //ABSTRACT_FACTORY_PATTERN_SHOE_H
