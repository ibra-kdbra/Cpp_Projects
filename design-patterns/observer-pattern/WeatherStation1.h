#ifndef OBSERVER_PATTERN_WEATHERSTATION1_H
#define OBSERVER_PATTERN_WEATHERSTATION1_H

#include "IObserver.h"


//This implementation is rather poor. In a real-world scenario it would probably be best
//if the observer holds a weak_ptr to the observable, and observable->remove is called
//in the observer destructor.


class WeatherStation1 : public IObserver<double> {
public:

    WeatherStation1(int _id): id(_id) {}

    void update(double val) {
        std::cout << "WEATHER STATION V1 (" << this->id << "): " << val << std::endl;
    }

private:
    int id;
};

#endif //OBSERVER_PATTERN_WEATHERSTATION1_H
