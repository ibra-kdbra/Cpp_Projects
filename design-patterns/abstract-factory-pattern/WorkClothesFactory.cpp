#include "WorkClothesFactory.h"
#include "DressShoes.h"
#include "ButtonedUpShirt.h"
#include "Slacks.h"

std::unique_ptr<Shoe> WorkClothesFactory::shoe() {
    return std::make_unique<DressShoes>();
}

std::unique_ptr<Shirt> WorkClothesFactory::shirt() {
    return std::make_unique<ButtonedUpShirt>();
}

std::unique_ptr<Pants> WorkClothesFactory::pants() {
    return std::make_unique<Slacks>();
}