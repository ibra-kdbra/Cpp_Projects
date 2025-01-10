# Co-C++19
## The Fastest Meta Programming Library

A versatile collection of compile time optimized tools for C++ programming.


## Target Compilers

This library uses the latest C++ features available in GCC, Clang and Visual Studio 2019 and 2022.
It compiles in "c++20" mode on all compilers.

Currently tested Compilers:
* Clang 12 to 15
* GCC 11, 12
* Visual Studio 2019 v16.11.x, 2022 v17.4.x

## Contents

Co-Cpp19 contains numerous sublibraries and is split into many small headers.
This aims to enable the "pay only what you use policy" of C++ at compile time.

* cpp19 - compiler configuration (yours should be somewhat similar)
* array19 - static and dynamic arrays, slices
* meta19 - utilities, type, index and pack wrappers and operations
* coro19 - basic coroutine with generator support
* strong19 - named strong types with tags
* enum19 - inspectable enum definitions
* string19 - compiletime string, view and string rope
* tuple19 - faster to compile `std::tuple` replacement
* optional19 - continuable optional and packed optional
* variant19 - faster compile and runtime `std::variant` replacement
* partial19 - densely packed and heap managed `tuple<optional<>>`
* serialize19 - customizable binary serialisation and deserialisation

Work in progress libraries, with less tests:

* signal19 - fast signal and slot handling
* lookup19 - ordered dynamic array lookup container

## Naming Schema

C++ uses a unified namespace between types, functions and constants.
Therefore we use the following patterns.

* `UpperCamelCasing` for types
* `lowerCamelCasing` for functions
* `UPPER_CASING` for precompiler definitions
* `lower_casing` for constexpr constants, concepts and namespaces

## Words and Phrases

* `*Of` - reflects something of a type
* `*At` - uses something at an index
* `[v]From()` - converts from arguments to v
* `*Type*` - indicates that the method is just a signature for usage with `decltype()`
