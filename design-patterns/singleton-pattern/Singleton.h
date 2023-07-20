#ifndef SINGLETON_PATTERN_SINGLETON_H
#define SINGLETON_PATTERN_SINGLETON_H

#include <string>

class Singleton {
public:

    static Singleton& instance();

    int get() const{
        return value;
    }

    int value;

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    Singleton(): value(0){}

};

#endif //SINGLETON_PATTERN_SINGLETON_H
