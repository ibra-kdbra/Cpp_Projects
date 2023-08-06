cmake_minimum_required(VERSION 3.16)
project(decorator_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(decorator_pattern main.cpp Beverage.h Espresso.cpp Espresso.h Mocha.cpp Mocha.h BeverageDecorator.h Soy.cpp Soy.h Caramel.cpp Caramel.h Cinnamon.cpp Cinnamon.h EspressoShot.cpp EspressoShot.h)