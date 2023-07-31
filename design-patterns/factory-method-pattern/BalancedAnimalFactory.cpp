#include "BalancedAnimalFactory.h"
#include "Cat.h"
#include "Dog.h"
#include "Duck.h"

std::shared_ptr<Animal> BalancedAnimalFactory::create() {
    this->next = (this->next + 1) % NUMBER_OF_ANIMALS;
    switch (this->next) {
        case 0:
            return std::make_shared<Cat>();
        case 1:
            return std::make_shared<Dog>();
        case 2:
            return std::make_shared<Duck>();
    }
    throw std::exception();
}