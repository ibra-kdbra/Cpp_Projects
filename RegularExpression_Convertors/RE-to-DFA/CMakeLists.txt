cmake_minimum_required(VERSION 3.9)
project(RegexToDFAConverter)

set(CMAKE_CXX_STANDARD 11)

add_executable(RegexToDFAConverter main.cpp dfa.h dfa.cpp regex_to_dfa.h regex_to_dfa.cpp)