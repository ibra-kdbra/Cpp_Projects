#include "EspressoShot.h"


std::string EspressoShot::description() {
    return "A shot of espresso, " + this->beverage->description();
}

double EspressoShot::cost() {
    return 1.5 + this->beverage->cost();
}