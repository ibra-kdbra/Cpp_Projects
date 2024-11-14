cmake_minimum_required(VERSION 3.25)
project(hangman)

set(CMAKE_CXX_STANDARD 17)

add_executable(hangman main.cpp)

find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)

# If using a custom install path, uncomment and set the include directory.
# include_directories("/path/to/sfml/include")

target_link_libraries(hangman sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
