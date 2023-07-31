#include <stdlib.h>
#include <time.h>

#include "RandomAnimalFactory.h"
#include "Cat.h"
#include "Dog.h"
#include "Duck.h"

std::shared_ptr<Animal> RandomAnimalFactory::create() {
    int next = rand() % NUMBER_OF_ANIMALS;
    switch (next) {
        case 0:
            return std::make_shared<Cat>();
        case 1:
            return std::make_shared<Dog>();
        case 2:
            return std::make_shared<Duck>();
    }
    throw std::exception();
}