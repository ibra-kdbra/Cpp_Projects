#include <iostream>
#include "AirConditioner.h"


void AirConditioner::turnOn() {
    if(not this->on){
        std::cout << "AC: Turned on! Temperature: " << this->temp << std::endl;
        this->on = true;
    }
}

void AirConditioner::turnOff() {
    if(this->on){
        std::cout << "AC: Turned off!" << std::endl;
        this->on = false;
    }
}

void AirConditioner::raiseTemp() {
    if(this->on && this->temp < this->HIGH_TEMP){
        this->temp++;
        std::cout << "New temp is: " << this->temp << std::endl;
    }
}

void AirConditioner::lowerTemp() {
    if(this->on && this->temp > this->LOW_TEMP){
        this->temp--;
        std::cout << "New temp is: " << this->temp << std::endl;
    }
}