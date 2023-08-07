cmake_minimum_required(VERSION 3.16)
project(command_pattern)

set(CMAKE_CXX_STANDARD 17)

add_executable(command_pattern main.cpp ICommand.h CommandNotFound.h ACRemote.cpp ACRemote.h RaiseTempCommand.cpp RaiseTempCommand.h TurnOnCommand.cpp TurnOnCommand.h AirConditioner.cpp AirConditioner.h ACNotAvailableException.h)