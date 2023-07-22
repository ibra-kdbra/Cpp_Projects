#ifndef OBSERVER_PATTERN_IOBSERVER_H
#define OBSERVER_PATTERN_IOBSERVER_H

template <typename T>
class IObserver {
public:
    virtual void update(T val) = 0;
};

#endif //OBSERVER_PATTERN_IOBSERVER_H
