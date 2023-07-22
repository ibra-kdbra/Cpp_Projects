#ifndef OBSERVER_PATTERN_WEATHERSTATION2_H
#define OBSERVER_PATTERN_WEATHERSTATION2_H

#include "IObserver.h"

class WeatherStation2 : public IObserver<double> {
public:

    WeatherStation2(int _id): id(_id) {}

    void update(double val) {
        std::cout << "WEATHER STATION V2 (" << this->id << "): " << val << std::endl;
    }

private:
    int id;
};

#endif //OBSERVER_PATTERN_WEATHERSTATION2_H
