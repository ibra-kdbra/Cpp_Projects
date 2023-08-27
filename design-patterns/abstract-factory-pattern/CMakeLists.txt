cmake_minimum_required(VERSION 3.16)
project(abstract_factory_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(abstract_factory_pattern main.cpp Shoe.h Shirt.h Pants.h CargoPants.cpp CargoPants.h Slacks.cpp Slacks.h TShirt.cpp TShirt.h ButtonedUpShirt.cpp ButtonedUpShirt.h Sneakers.cpp Sneakers.h DressShoes.cpp DressShoes.h ClothesFactory.h WorkClothesFactory.cpp WorkClothesFactory.h DayOffClothesFactory.cpp DayOffClothesFactory.h)