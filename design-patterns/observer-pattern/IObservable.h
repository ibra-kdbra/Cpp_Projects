#ifndef OBSERVER_PATTERN_IOBSERVABLE_H
#define OBSERVER_PATTERN_IOBSERVABLE_H

#include "IObserver.h"

template<typename T>
class IObservable {
public:
    virtual void add(std::weak_ptr<IObserver<T>> observer) = 0;
    virtual void remove(std::weak_ptr<IObserver<T>> observer) = 0;
private:
    virtual void notify() = 0;
};

#endif //OBSERVER_PATTERN_IOBSERVABLE_H
