#include "Caramel.h"

std::string Caramel::description() {
    return "Caramel, " + this->beverage->description();
}

double Caramel::cost() {
    return 1 + this->beverage->cost();
}