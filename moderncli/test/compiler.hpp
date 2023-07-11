// Copyright (c) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef	COMPILER_HPP_
#define	COMPILER_HPP_

#include "config.hpp"

namespace tycho {}
using namespace tycho;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wunused-member-function"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#if !defined(_MSC_VER) && __cplusplus < 201703L
#error C++17 compliant compiler required
#endif

#ifdef  OS_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif

template<typename T>
inline auto is(const T& object) {
    return object.operator bool();
}

/*!
 * \mainpage ModernCLI
 * A portable set of stand-alone command line utilities and header-only
 * libraries I may vendor in other projects. Some may use openssl.
 * \author David Sugar <tychosoft@gmail.com>
 */

/*!
 * Compiler and platform specific definitions.
 * \file compiler.hpp
 */
#endif
