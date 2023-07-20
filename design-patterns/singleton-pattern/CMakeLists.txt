cmake_minimum_required(VERSION 3.16)
project(singleton_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(singleton_pattern main.cpp Singleton.h Singleton.cpp)