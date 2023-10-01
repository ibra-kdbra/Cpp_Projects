cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 20)

project(game)

find_package(SDL2 REQUIRED)
find_package(TBB REQUIRED)

include_directories(./include/)


add_executable(game src/main.cpp
                    src/gamehandler.cpp
                    src/input.cpp
                    src/gamemanager.cpp
                    src/object.cpp
                    src/tasker.cpp
                )

target_link_libraries(game ${SDL2_LIBRARIES} ${CMAKE_SOURCE_DIR}/libs/libPathfinder.a tbb)
