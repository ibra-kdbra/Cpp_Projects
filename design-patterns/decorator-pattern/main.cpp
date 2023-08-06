#include <iostream>
#include "Soy.h"
#include "Espresso.h"
#include "Caramel.h"
#include "Cinnamon.h"
#include "EspressoShot.h"
#include "Mocha.h"

int main() {
    auto order1 = std::make_unique<Soy>(std::make_unique<Caramel>(std::make_unique<Espresso>()));
    auto order2 = std::make_unique<Caramel>(std::make_unique<Cinnamon>(std::make_unique<EspressoShot>(std::make_unique<Soy>(std::make_unique<Mocha>()))));
    std::cout << "Order 1: " << order1->description() << " : " << order1->cost() << std::endl;
    std::cout << "Order 2: " << order2->description() << " : " << order2->cost() << std::endl;
    return 0;
}
