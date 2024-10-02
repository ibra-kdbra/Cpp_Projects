# C++ Projects **PlayGround** #

### I made this repo to make more C++ projects and implement new things I always wanted to try

### **This repo** will contain some forked or cloned repos from other users, their credit will be found in every *README* of each directory and it will be edited to my need

### I hope this repository never stops growing and teaches me new things about this fascinating programming language

# Table of Contents

[Overview](#overview)

[Side note](#side-note)

[Projects:](#projects)

- [Atari Games](#atrai-games)
- [Beneficial C Programming](#beneficial-c_programming)
- [CanChat](#canchat)
- [Chess-Engine](#chess-engine)
- [Chess_3D](#chess_3d)
- [Collatz Conjecture](#collatz-conjecture)
- [CPP20_Develop](#cpp20_develop)
- [CustomizedOperators](#customizedoperators)
- [Define_evil](#define_evil)
- [Design-patterns](#design-patterns)
- [Dump C++](#dump-c)
- [FileLockSocket](#filelocksocket)
- [Hangman-game](#hangman-game)
- [InjectHook](#injecthook)
- [Introspective](#introspective)
- [Javascript-compiler](#javascript-compiler)
- [JsonConverter](#jsonconverter)
- [MarbleMarcher](#marblemarcher)
- [Matrix_ScreenSaver](#matrix_screenSaver)
- [MiniShell](#minishell)
- [Mothbar](#mothbar)
- [OfxAsync](#ofxasync)
- [OpenGL_examples](#opengl_examples)
- [PackSend](#packsend)
- [Password_generator](#password_generator)
- [Pipes_ScreenSaver](#pipes_screenSaver)
- [POng](#POng)
- [Random Utility tools](#random_utility_tools)
- [Regular Expression Convertors](#regular_expression_convertors)
- [Simple_turso](#simple_turso)
- [Simple-Code](#simple-code)
- [Simplesdl2_game_template](#simplesdl2_game_template)
- [StaticPageGen](#staticpagegen)
- [SystemMonitor](#systemmonitor)
- [Template Language Generator](#template_language_generator)
- [Terminal File Manager](#terminal_file_manager)
- [Terminal Snake](#terminal_snake)
- [Thread Pool](#thread_pool)
- [Tokenizer_tok](#tokenizer_tok)
- [Trigonometric Functions](#trigonometric-functions)
- [ViteFA_config](#vitefa_config)
- [Volumetric-clouds](#volumetric-clouds)
- [WordProc](#wordproc)


## Overview
This repository serves as a personal playground for experimenting with various C++ projects. It includes original implementations, as well as forked or cloned repositories from other developers with my own improvements as fits. Each project aims to explore different concepts and features of the C++ programming language.


### Side note:
Projects are organized alphabetically for easy navigation. Each corresponding *README* file provides specific details, usage instructions, and credits for any borrowed code.
#

## Projects

### [Atrai-Games](./Atari-Games)

A collection of classic Atari games implemented in C++:

1. 2048 - [Learn more](https://en.wikipedia.org/wiki/2048_(video_game))

2. Snake - [Learn more](https://en.wikipedia.org/wiki/Snake_(video_game_genre))

3. Tetris - [Learn more](https://en.wikipedia.org/wiki/Tetris)

<a href="#table-of-contents"> Back to TOC ⤴️ </a>

#

### [Beneficial-C_programming](./Beneficial-C_programming)
*Compile and execute C "scripts" in one go!*

 A shell script designed to streamline the process of compiling and executing C (and C++) scripts with unparalleled speed. This tool allows users to run C code directly from the command line or via shebang, making it as convenient as running shell scripts. With support for multiple files and customizable compiler options.
 
#

### [CanChat](./canChat)
Feature-complete rooms-based chat for tens of thousands of users. Client plus Server. Built with [Oat++ Web Framework](https://oatpp.io/).
`Single-Node` `In-Memory` `WebSocket` `TLS`.

#
### [Chess_3D](./Chess_3D)

A simple 3D chess game that depends on GLFW and GLM, with U-R-C-F for game controls.

#
### [CPP20_Develop](./CPP20_Develop)

`A versatile collection of compile-time optimized tools for C++ programming.`

This library uses the latest C++ features available in GCC, Clang and Visual Studio 2019 and 2022.

#

### [Chess-Engine](./Chess_Engine)

Chess Engine in C++17.
#

### [Collatz-Conjecture](./Collatz)

[Learn more](https://en.wikipedia.org/wiki/Collatz_conjecture) about Collatz Conjecture.

A program that attempts to solve Collatz-Conjecture question which asks whether repeating two simple arithmetic operations will eventually transform every positive integer into one.

#

### [CustomizedOperators](./CustomizedOperators)

Implements custom operators for C++.
#### Like what?
In C++ one can not create new operators such as <> or |&. What you can do however is create a variable named x of newly created type with irrelevant name and overload the operators < and > such that you can write <x> creating a new operators of sorts.
#

### [Define_evil](./define_evil)

*A macro library for the morally compromised C and C++ programmer*

It's an MIT licensed collection of clean, modular, well documented macro hacks with full unit test coverage.
Complex macros are useful when pushing the limits of what C/C++ can be.
#

### [Design-patterns](./design-patterns)

[Object Oriented Design](https://en.wikipedia.org/wiki/Object-oriented_analysis_and_design#Object-oriented_design) patterns in C++ based on the the book [Head First Design Patterns: A Brain-Friendly Guide](https://www.amazon.com/Head-First-Design-Patterns-Brain-Friendly/dp/0596007124).

#### Implemented design patterns:
 - Abstract factory pattern
 - Command pattern
 - Decorator pattern
 - Factory method pattern
 - Observer pattern
 - Singleton pattern
 - Strategy pattern

#
### [Dump C++](./Dump_c%2B%2B)

*Dump C++ Itanium [ABI](https://en.wikipedia.org/wiki/Application_binary_interface) from [ELF](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) objects*

A tool used to inspect C++ binaries for debug symbols ([Learn more](https://en.wikipedia.org/wiki/Debug_symbol)) and virtual function tables ([Learn more](https://en.wikipedia.org/wiki/Virtual_method_table)) information.
#

### [FileLockSocket](./FileLockSocket)

*Transfer files quickly and securely!*

The project utilizes [PBKDF2](https://en.wikipedia.org/wiki/PBKDF2) for key derivation, employs AES-256-GCM for encryption/decryption, and uses [SHA3-256](https://en.wikipedia.org/wiki/SHA-3) for integrity check.
#

### [Hangman-game](./Hangman-game)

**What is hangman game?** [Learn more](https://en.wikipedia.org/wiki/Hangman_(game))

A text based game, the program takes in 2 parametes, the first one is a word or an expression, the second one is the number of attempts you want in your game.
#

### [InjectHook](./InjectHook)

A lightweight C++ library designed for function interception within injected DLLs, providing a streamlined approach to modifying application behavior at runtime. Ideal for educational purposes, debugging, and dynamic software analysis.
#

### [introspective](./introspective)

Introspective is a header file that brings reflection to any class that wants it, regardless of whether the reflected member is a constant, a static variable or a instance member function. It records declaration order, (function) type and address and passes them along unchanged during compile-time, with the ultimate goal of making the interaction with embedded scripting languages like Lua a little less of a hassle.
#

### [javascript-compiler](./javascript-compiler)
The goal of the project development was learning the mechanism of compilers. The project contains 5 parts:

1) a lexer;

2) a parser;

3) an interpreter;

4) a compiler;

5) a virtual machine;

The designed programming language is similar to Javascript, but much simpler. Code samples can be found in each part's testcase/* .
#

### [JsonConverter](./JsonConverter)
A C++ Library providing a `std::string` parser for JavaScript Object Notation aka JSON.
The STL containers are in favor to represent the Json entities: Object, Array and Value.
#

### [MarbleMarcher](./MarbleMarcher)

Marble Marcher is a video game demo that uses a fractal physics engine and fully procedural rendering to produce beautiful and unique gameplay unlike anything you've seen before.

The goal of the game is to reach the flag as quickly as possible.  But be careful not to
fall off the level or get crushed by the fractal!  There are 24 levels to unlock.
#

### [Matrix_ScreenSaver](./Matrix_ScreenSaver)

Matrix terminal screen saver made in C language.

&emsp;**Screenshot:**

&emsp;<img src="Matrix_ScreenSaver/screenshots/matrix.png" width="400">

#

### [MiniShell](./miniShell)
#

### [Mothbar](./mothbar)
Small pieces of code to build a [FreeBSD](https://en.wikipedia.org/wiki/FreeBSD) bar.
#

### [OfxAsync](./ofxAsync)
An [openFrameworks](https://openframeworks.cc/) addon compatible with versions 0.9.8 to 0.10.1, designed to simplify multi-threading through a straightforward wrapper around [ofThread](https://openframeworks.cc/documentation/utils/ofThread/).
#

### [OpenGL_examples](./OpenGL_examples)
A group of examples done upon taking a Computer Graphics course to better understand OpenGL and apply some of the techniques read in the litterature.

&emsp; **Screenshots:**

&emsp;
<img src="./OpenGL_examples/screenshots/grass_1.png" width="100"> <img src="./OpenGL_examples/screenshots/shadow_mapping_2.png" width="100"> <img src="./OpenGL_examples/screenshots/water_1.png" width="100"> <img src="./OpenGL_examples/screenshots/tree_2.png" width="100"> <br>
&emsp; <img src="./OpenGL_examples/screenshots/clouds_1.png" width="150"> <img src="./OpenGL_examples/screenshots/lod_terrain_2.png" width="150"> <img src="./OpenGL_examples/screenshots/volumetric_light_2.png" width="100">
#

### [PackSend](./packsend/)
A small utility to construct packets from various data sources and output
them to sockets, serial ports or files.
#

### [Password_generator](./password-generator)
A CLI password generator coded in C++.
#

### [Pipes_ScreenSaver](./Pipes_ScreenSaver)

Pipes terminal screen saver made in C language.

&emsp;**Screenshot:**

&emsp;<img src="Pipes_ScreenSaver/screenshots/pipes.png" width="400">

#

### [POng](./POng)

Pong ([learn more](https://en.wikipedia.org/wiki/Pong)) clone for Linux written in C++ with SDL 2.0.
#

&emsp; **Screenchot:**

&emsp; <img src="https://cloud.githubusercontent.com/assets/1498164/5608125/295a3186-9481-11e4-968b-04987a925a93.png" width="400">
#

### [Random_Utility_tools](./Random_Utility_tools)

#
### [Regular_Expression_Convertors](./RegularExpression_Convertors)

There are three convertors:

- Regular Expression to Deterministic Finite Automata
- Regular Expression to Non-deterministic Finite Automata
- Regular Expression to Epsilon Non-deterministic Finite Automata

#

### [Simple_turso](./simple_turso)

Experimental 3D / game engine technology partially based on the Urho3D codebase.
#

### [Simple-Code](./Simple-Code)

A collection of algorithms and data structures made with C++ language.

#

### [Simplesdl2_game_template](./simplesdl2_game_template)
#

### [StaticPageGen](./staticpagegen)

staticpagegen is a tool used to generate static pages for directory hierarchies
which uses the simplest possible form for the protocols it supports.
# 

### [SystemMonitor](./SystemMonitor)

This repository contains the result of the second project of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). The project concerns a system monitor for the Linux Operating System, which depends on [ncurses](https://www.gnu.org/software/ncurses/) library.

#

### [Template_Language_Generator](./Template_Language_Generator)

A small POSIX shell script for generating a C/C++ code structure, build system files, etc.

#

### [Terminal_File_Manager](./Terminal_File_Manager)


Terminal file manager in C++20 (and [ncurses](https://www.gnu.org/software/ncurses/)).

#

### [Terminal_Snake](./Terminal_Snake)

`tsnake` is a terminal snake game written in C++ using the [ncurses](https://www.gnu.org/software/ncurses/) library with
support for maps and interactive resizing.

&emsp;**Preview:**

&emsp;<img src="Terminal_Snake/img/tsnake.gif" width="400">

#

### [Thread_Pool](./Thread_Pool)

A thread pool is a software design pattern for achieving concurrency of execution in a computer program ([learn more](https://en.wikipedia.org/wiki/Thread_pool)).
The project is a simple async thread pool implementation in C++ language.

#

### [Tokenizer_tok](./Tokenizer_tok)

A simple, dependency-free, stb-style
single header *source code tokenizer* written in ANSI C. It has a simple to use interface to manipulate the tokens and then
concatenate them into a string again.

#

### [Trigonometric-Functions](./Trigonometric-Functions)


A small program that lets you visualize multipe trigonometric functions, built in C++ with OpenGL and GLUT.

&emsp; **Preview:**

&emsp;<img src="./Trigonometric-Functions/assets/preview.gif" width="400"/>

#

### [ViteFA_config](./ViteFA_config)

#

### [Volumetric-clouds](./volumetric-clouds)
#

### [WordProc](./WordProc)


An attempt to write a word processor using **C++20**, **Qt5** and cmake
as buildsystem.

#
Update, made two related screen savers with C-programming language(Pipes_ScreenSaver)(Matrix_ScreenSaver), and also trained with some algorithms in the SimpleCode directory, this will be helpful.

Added the Marble Marcher game(MarbleMarcher) which has been made by the Code-Paradoy YouTube channel and changed some things to it.

Made template for c/c++ library or executed files(Template_Language_Generator), support desing modules(support_modules),System Cpu Monitor(SystemMonitor),tokenizer for c/c++, terminal file manager(tfm)

Made a shell with c low-level programming language.(miniShell)

Pushed (Dump_c++) doc and (ModerCLI) which I don't own, left the credits in the markdown file.

Made custom operator with C++ for customizing my needs.(CustomizedOperator)

Pushed a header file that brings reflection to any class that wants it, regardless of
whether the reflected member is a constant(introspective)

Finished the Word Processor with qt5 finally(WordProc)

Building an improved version of c++ 19/20 for my uses(CPP20Develop)

Trying with some simple game engine trying to figure out things on my own(simple_turso)=finished

Added a similar javascript interpreter(javascript-compiler)

Customized a C_programming language(Benefetial-C_programming)

Added simple thread wrapper for easier multiThreading(ofxAsync)

Added simple async thread pool implementation(Thread_Pool)

Added automata loops(Resgular_Expressions)

Added injection for the app(Injection)

Added a confined simple and very secure chat app with otpp(canChat)

Pushed safeguard for the Windows system(safeGuards)
