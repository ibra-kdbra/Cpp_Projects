cmake_minimum_required(VERSION 3.15)
project(cocpp19-optional19)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
find_package(GTest REQUIRED)
find_package(CoCpp19 COMPONENTS optional19 REQUIRED)

enable_testing()

include("../../../src/optional19.lib/optional19/optional19.tests.cmake")
