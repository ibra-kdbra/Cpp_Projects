cmake_minimum_required(VERSION 3.15)
project(cocpp19-lookup19)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
find_package(GTest REQUIRED)
find_package(CoCpp19 COMPONENTS lookup19 REQUIRED)

enable_testing()

include("../../../src/lookup19.lib/lookup19/lookup19.tests.cmake")
