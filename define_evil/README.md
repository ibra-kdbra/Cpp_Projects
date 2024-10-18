# `#define EVIL`

*A macro library for the morally compromised C and C++ programmer.*

## What is this?

There's one thing everyone can agree on, macros in C are a necessary evil. At first glance, they're a quick-and-dirty way of defining constants, but take a closer look, and you'll find a disturbing truth. Macros can consume arbitrary numbers of arguments, expand to other macros, perform logic and can almost-sorta-kinda be recursive, all with complete disregard of the syntactic correctness of the code they are expanding to. Welcome to a world where everything could be anything and nothing is as it seems. Welcome to `#define EVIL`.

## No, actually, what is this?

It's an MIT-licensed collection of clean, modular, well-documented macro hacks with full unit test coverage.

## Why?

Complex macros are useful when pushing the limits of what C/C++ can do. For example, you could use `#define EVIL` macros to construct a domain-specific language that is technically C or C++ (compiled by any normal compiler) but looks and feels quite different. An extreme example of this is [Python++](https://github.com/wmww/Python-plus-plus), where many of these ideas originated. `#define EVIL` was created to split out and clean up the reusable bits of Python++.

## Files

* [define_EVIL.h](define_EVIL.h): the macros that are maintained by hand
* [generated_EVIL.h](generated_EVIL.h): auto generated macros (if you see a macro starting with `_EVIL_GEN_`, its in this file)
* [generator.sh](generator.sh): a BASH script that generates generated_EVIL.h
* [tests.c](tests.c): unit tests

## Running tests

To run the tests, just clone this repo, `cd` into it and run `make test`. Make will run generator.sh if needed.

## Using

To use in your C or C++ project, simply copy `define_EVIL.h` and `generated_EVIL.h` into your project and `#include "define_EVIL.h"`. You may want to copy the contents of `generated_EVIL.h` into `define_EVIL.h` to make managing both easier.

You can also grab only the individual macros you need. The dependencies of the simpler ones are documented to make this easier.

To see what macros are available and how they work you can either read the inline docs in the main header or look at the tests. It's a little perplexing at first but really not too bad (just don't try to understand how the complicated ones work, you'll regret it).

## Namespacing

All #define EVIL macros start with either `EVIL` or `_EVIL` (the former for ones that are expected to appear in user code, the latter for ones that are used internally). Because of how macros work, it's not possible to keep them out of the global namespace (even in C++).
