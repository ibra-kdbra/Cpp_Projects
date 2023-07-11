cmake_minimum_required(VERSION 3.14)
project(tfm CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -Wsign-conversion -Wshadow")

# ncurses
include(FindCurses)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses)

add_executable(tfm src/main.cpp)
target_link_libraries(tfm ${CURSES_LIBRARIES})
