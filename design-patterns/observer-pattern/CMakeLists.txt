cmake_minimum_required(VERSION 3.16)
project(observer_pattern)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

add_executable(observer_pattern main.cpp IObserver.h WeatherMeasurementService.h IObservable.h WeatherMeasurementService.cpp WeatherStation1.h WeatherStation2.h)