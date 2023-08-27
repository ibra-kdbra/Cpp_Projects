#include <iostream>
#include "WorkClothesFactory.h"
#include "DayOffClothesFactory.h"

int main() {
    std::cout << "Going to work:" << std::endl;

    WorkClothesFactory wcf;
    std::cout << wcf.shoe()->clean() << std::endl;
    std::cout << wcf.shirt()->putOn() << std::endl;
    std::cout << wcf.pants()->description() << std::endl;
    std::cout << wcf.pants()->numOfPockets() << std::endl;
    std::cout << std::endl;

    std::cout << "Taking a day off:" << std::endl;

    DayOffClothesFactory docf;
    std::cout << docf.shoe()->clean() << std::endl;
    std::cout << docf.shirt()->putOn() << std::endl;
    std::cout << docf.pants()->description() << std::endl;
    std::cout << docf.pants()->numOfPockets() << std::endl;

    return 0;
}
