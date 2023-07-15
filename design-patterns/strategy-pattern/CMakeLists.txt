cmake_minimum_required(VERSION 3.16)
project(strategy_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(strategy_pattern main.cpp IQuackBehavior.h IFlyBehavior.h Duck.h SimpleQuackBehavior.cpp SimpleQuackBehavior.h SimpleFlyingBehavior.cpp SimpleFlyingBehavior.h NoFlyingBehavior.h FancyFlyingBehavior.cpp FancyFlyingBehavior.h CityDuck.h NoQuackBehavior.h CountryDuck.h FreeFlyingBehavior.h)