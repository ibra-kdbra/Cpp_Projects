#include "Soy.h"


std::string Soy::description() {
    return "Soy, " + this->beverage->description();
}

double Soy::cost() {
    return 0.5 + this->beverage->cost();
}