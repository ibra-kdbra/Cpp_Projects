#include <iostream>
#include "Duck.h"
#include "SimpleFlyingBehavior.h"
#include "SimpleQuackBehavior.h"
#include "CityDuck.h"
#include "CountryDuck.h"

int main() {
    std::cout << "Simple Duck: " << std::endl;
    Duck simpleDuck{std::make_shared<SimpleQuackBehavior>(), std::make_shared<SimpleFlyingBehavior>()};
    simpleDuck.quack();
    simpleDuck.fly();

    std::cout << "\nCity Duck: " << std::endl;
    CityDuck cityDuck{};
    cityDuck.quack();
    cityDuck.fly();

    std::cout << "\nCountry Duck: " << std::endl;
    CountryDuck countryDuck{};
    countryDuck.quack();
    countryDuck.fly();

    return 0;
}
