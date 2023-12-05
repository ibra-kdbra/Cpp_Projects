cmake_minimum_required (VERSION 2.6)
add_compile_options(-std=c++11)
project (ThreadPool)

add_executable(
    main 
    test.cpp
    ThreadPool.h
)

