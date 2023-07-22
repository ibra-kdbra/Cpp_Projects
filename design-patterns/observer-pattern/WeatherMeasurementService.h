#ifndef OBSERVER_PATTERN_WEATHERMEASUREMENTSERVICE_H
#define OBSERVER_PATTERN_WEATHERMEASUREMENTSERVICE_H

#include "IObservable.h"
#include <set>
#include <vector>

class WeatherMeasurementService : public IObservable<double> {
public:
    WeatherMeasurementService() :temperature(0) {}

    void add(std::weak_ptr<IObserver<double>> observer);

    void remove(std::weak_ptr<IObserver<double>> observer);

    void addTemp(double delta);

    void change(double _temperature);

private:
    void notify();

    double temperature;
    //because we are using weak_ptr (since it makes no sense that observable
    //will have ownership of its observer) we are using a vector and not a set
    //or unordered_set as in previous commits, since weak_ptrs cannot be hashed
    //unless locked (i.e transformed to shared_ptr) and when invalidated their
    //hash will change. Probably this can be done with shared_ptr and reference
    //counting... but this is just an example so we'll live with it.
    std::vector<std::weak_ptr<IObserver<double>>> observers;
};

#endif //OBSERVER_PATTERN_WEATHERMEASUREMENTSERVICE_H
