#include <iostream>
#include "WeatherMeasurementService.h"

void WeatherMeasurementService::add(std::weak_ptr<IObserver<double>> observer) {
    std::cout << "ADDING WEATHER STATION" << std::endl;
    if(auto shared = observer.lock()){
        this->observers.push_back(observer);
        shared->update(this->temperature);
    }
}
void WeatherMeasurementService::remove(std::weak_ptr<IObserver<double>> observer) {
    std::cout << "REMOVING WEATHER STATION" << std::endl;
    this->observers.erase(std::find_if(this->observers.begin(), this->observers.end(),
            [observer](std::weak_ptr<IObserver<double>> ptr) {
        return observer.lock() == ptr.lock();
    }));
};

void WeatherMeasurementService::addTemp(double delta){
    this->temperature += delta;
    this->notify();
}

void WeatherMeasurementService::change(double _temperature){
    this->temperature = _temperature;
    this->notify();
}

void WeatherMeasurementService::notify() {
    std::vector<std::weak_ptr<IObserver<double>>> toRemove;
    for(auto observer : this->observers){
        if(auto obs = observer.lock()) {
            obs->update(this->temperature);
        } else {
            toRemove.push_back(observer);
        }
    }
};
