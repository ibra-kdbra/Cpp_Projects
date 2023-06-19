cmake_minimum_required(VERSION 3.13)

project(example_JsonPP LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ccdjson_example_fromString src/fromString.cpp)
add_executable(ccdjson_example_fromCode src/fromCode.cpp)
add_executable(ccdjson_example_accessValues src/accessValues.cpp)

find_package(ccdJsonpp 1.0 REQUIRED)
target_link_libraries(ccdjson_example_fromString PUBLIC ccdJsonpp)
target_link_libraries(ccdjson_example_fromCode PUBLIC ccdJsonpp)
target_link_libraries(ccdjson_example_accessValues PUBLIC ccdJsonpp)

