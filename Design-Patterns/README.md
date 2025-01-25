Design Patterns
---

<!-- 

this file should answer the following questions:
1. Overview (explains what this specific project does)

while the following questions seem to be common knowledge, it's better to include a brief explanation of them, to keep the reader on our page, and not lose them to the sea of wide internet...

2. What are design patterns? [x]
3. Why design patterns? [x]
4. Who set them? [x]

5. Contents of the folders?
6. Requirements and build (dependencies)

Due 2025, February 4th
 -->
# Overview

> _UNDER CONSTRUCTION_

<img src="https://wiki.archiveteam.org/images/8/8b/Underconstruction.gif" width="200" alt="under constructon sign">


# What, Why, and Who?

## What are Design Patterns?

Design patterns are typical solutions to commonly occurring problems in software design. They are like pre-made blueprints that you can customize to solve a recurring design problem in your code.

All patterns can be categorized by their _intent_, or purpose. This book covers three main groups of patterns:

- <b>Creational patterns</b> provide object creation mechanisms that increase flexibility and reuse of existing code.

- <b>Structural patterns</b> explain how to assemble objects and classes into larger structures, while keeping these structures flexible and efficient.

- <b>Behavioral patterns</b> take care of effective communication and the assignment of responsibilities between objects.

## Why Design Patterns?

Because design patterns are a toolkit of tried and tested solutions to common problems in software design.
They're like learning color blends instead of just trying out every pair of colors to get a wanted color (solution).

## Who came up with Design Patterns?

 A history lesson here, 
 
**Short answer:** Erich Gamma, John Vlissides, Ralph Johnson, and Richard Helm, back in 1994.

**Better answer:** 
 
Design patterns gained popularity in computer science after the book Design Patterns: Elements of Reusable Object-Oriented Software was published in 1994 by the so-called "Gang of Four" (Erich Gamma, Richard Helm, Ralph Johnson and John Vlissides), which is frequently abbreviated as "GoF".

Learn more on design patterns at <a href="https://refactoring.guru/design-patterns">Refactoring.Guru</a> and <a href="https://en.wikipedia.org/wiki/Software_design_pattern">Wikipedia</a>.

# Contents of Directories
<!-- need to come up with a better title -->
Implemented Design Patterns so far:

- Behavioral Patterns
    - Chain of responsibility
    - Command
    - Iterator aka Cursor
    - Observer
    - Strategy
    - Template Method

- Creational Patterns
    - Abstract Factory
    - Builder
    - Factory Method
    - Prototype
    ...
    <!-- TODO: research -->

# Requirements and Build

Requirements:
- Cmake
- C++ compiler
<!-- TODO: specify more -->

## How to build
Each category has its own cmake file, hence you will need to change directory to the desired category `cd`, run the following commands one by one:

```sh
cd {Category_dir}/
cmake -S . -B build
cmake --build ./build
...
```
<!-- TODO: specify what to run next -->

> [!NOTE]
> Replace {Category_dir} with the actual directory name.

<!-- TODO: README file for each category and then redirect reader to each build section of corresponding README -->