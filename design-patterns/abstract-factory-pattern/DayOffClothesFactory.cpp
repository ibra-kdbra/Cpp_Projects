#include "DayOffClothesFactory.h"
#include "Sneakers.h"
#include "TShirt.h"
#include "CargoPants.h"

std::unique_ptr<Shoe> DayOffClothesFactory::shoe() {
    return std::make_unique<Sneakers>();
}

std::unique_ptr<Shirt> DayOffClothesFactory::shirt() {
    return std::make_unique<TShirt>();
}

std::unique_ptr<Pants> DayOffClothesFactory::pants() {
    return std::make_unique<CargoPants>();
}