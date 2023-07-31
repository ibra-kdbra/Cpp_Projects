#include <iostream>
#include "BalancedAnimalFactory.h"
#include "RandomAnimalFactory.h"

int main() {
    std::cout << "IN ORDER:" << std::endl;
    BalancedAnimalFactory baf;
    for(int i = 1 ; i < 10 ; i++){
        auto animal = baf.create();
        std::cout << animal->sound() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "RANDOM ORDER:" << std::endl;
    RandomAnimalFactory raf;
    for(int i = 1 ; i < 10 ; i++){
        auto animal = raf.create();
        std::cout << animal->sound() << std::endl;
    }

    return 0;
}
