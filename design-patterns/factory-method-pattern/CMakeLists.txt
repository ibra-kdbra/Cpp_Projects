cmake_minimum_required(VERSION 3.16)
project(factory_method_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(factory_method_pattern main.cpp Animal.h Cat.h Dog.h Duck.h AnimalFactory.h BalancedAnimalFactory.cpp BalancedAnimalFactory.h RandomAnimalFactory.cpp RandomAnimalFactory.h)